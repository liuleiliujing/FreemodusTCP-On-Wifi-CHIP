#!/bin/bash

# This script will start the compilation of cm4/dsp according to the configuration file (mapping_proj.cfg).
# Build cm4 would bring bl option by default.

###############################################################################
# Variables
OUT="$PWD/out"
working_dir="$PWD"
feature_mk=""
source mcu/tools/scripts/build/co_build_script/mapping_proj.cfg
###############################################################################
# Functions
show_usage () {
    echo "==============================================================="
    echo "Build Project"
    echo "==============================================================="
    echo "Usage: $0 <board> <project> [clean]"
    echo ""
    echo "Example:"
    echo "       $0 ab1558_evk no_rtos_initialize_system"
    echo "       $0 clean                                          (clean folder: out)"
    echo "       $0 ab1558_evk clean                              (clean folder: out/ab1558_evk)"
    echo "       $0 ab1558_evk no_rtos_initialize_system clean    (clean folder: out/ab1558_evk/no_rtos_initialize_system)"
    echo ""
    echo "==============================================================="
    echo "List Available Example Projects"
    echo "==============================================================="
    echo "Usage: $0 list"
    echo ""
}

show_available_proj () {
    echo "==============================================================="
    echo "Available Build Projects:"
    echo "==============================================================="
    for var in ${!map_@}
    do
        board=$(echo $var | sed 's/__/:/g' | cut -d':' -f2)
        project=$(echo $var | sed 's/__/:/g' | cut -d':' -f3)
        echo "$board"
        echo "  $project"
        eval proj_map_key=( \${!$var[@]} )
        eval proj_map_value=( \${$var[@]} )
        for key in ${proj_map_key[@]}
        do
            eval echo \"\ \ \ \ \ \ $key: \${$var[\"$key\"]}\"
        done
    done
}

clean_out () {
    rm -rf $1
    echo "rm -rf $1"
}

# copy cm4 files
copy_cm4_files () {
    debug_dir=out/$1/$3/debug
    download_dir=out/$1/$3/download
    if [ ! -e "$debug_dir" ]; then
        mkdir -p "$debug_dir"
    fi
    if [ ! -e "$download_dir" ]; then
        mkdir -p "$download_dir"
    fi
    # copy debug files if exist
    echo "cp -f mcu/out/$1/$2/$2.elf  $debug_dir"
    echo "cp -f mcu/out/$1/$2/$2.dis  $debug_dir"
    echo "cp -f mcu/out/$1/$2/$2.map  $debug_dir"
    echo "cp -f mcu/out/$1/$2/cm4_log_str.bin  $debug_dir"
    test -e mcu/out/$1/$2/$2.elf && cp -f mcu/out/$1/$2/$2.elf  $debug_dir
    test -e mcu/out/$1/$2/$2.dis && cp -f mcu/out/$1/$2/$2.dis  $debug_dir
    test -e mcu/out/$1/$2/$2.map && cp -f mcu/out/$1/$2/$2.map  $debug_dir
    test -e mcu/out/$1/$2/cm4_log_str.bin && cp -f mcu/out/$1/$2/cm4_log_str.bin  $debug_dir

    # copy download files if exist
    echo "cp -f mcu/out/$1/$2/flash_download.cfg $download_dir"
    echo "cp -f mcu/out/$1/$2/*_bootloader.bin $download_dir"
    echo "cp -f mcu/out/$1/$2/$2.bin $download_dir"
    echo "cp -f mcu/out/$1/$2/partition_table.bin $download_dir"
    echo "cp -f mcu/out/$1/$2/ab155x_patch_hdr.bin $download_dir"
    test -e mcu/out/$1/$2/flash_download.cfg && cp -f mcu/out/$1/$2/flash_download.cfg $download_dir
    test -e mcu/out/$1/$2/*_bootloader.bin && cp -f mcu/out/$1/$2/*_bootloader.bin $download_dir
    test -e mcu/out/$1/$2/$2.bin && cp -f mcu/out/$1/$2/$2.bin $download_dir
    test -e mcu/out/$1/$2/partition_table.bin && cp -f mcu/out/$1/$2/partition_table.bin $download_dir
    test -e mcu/out/$1/$2/ab155x_patch_hdr.bin && cp -f mcu/out/$1/$2/ab155x_patch_hdr.bin $download_dir
}

# copy dsp files
copy_dsp_files () {
    debug_dir=out/$1/$3/debug
    download_dir=out/$1/$3/download
    if [ ! -e "$debug_dir" ]; then
        mkdir -p $debug_dir
    fi
    if [ ! -e "$download_dir" ]; then
        mkdir -p "$download_dir"
    fi
    # copy debug files if exist
    echo "cp -f dsp/out/$1/$2/$2.out  $debug_dir"
    echo "cp -f dsp/out/$1/$2/$2.asm  $debug_dir"
    echo "cp -f dsp/out/$1/$2/$2.map  $debug_dir"
    echo "cp -f dsp/out/$1/$2/dsp*_log_str.bin  $debug_dir"
    test -e dsp/out/$1/$2/$2.out && cp -f dsp/out/$1/$2/$2.out  $debug_dir
    test -e dsp/out/$1/$2/$2.asm && cp -f dsp/out/$1/$2/$2.asm  $debug_dir
    test -e dsp/out/$1/$2/$2.map && cp -f dsp/out/$1/$2/$2.map  $debug_dir
    test -e dsp/out/$1/$2/dsp*_log_str.bin && cp -f dsp/out/$1/$2/dsp*_log_str.bin  $debug_dir

    # copy download files if exist
    echo "cp -f dsp/out/$1/$2/$2.bin $download_dir"
    test -e dsp/out/$1/$2/$2.bin && cp -f dsp/out/$1/$2/$2.bin $download_dir
}

###############################################################################
# Parsing arguments
if [ "$#" -eq "0" ]; then
    show_usage
    exit 1
fi

declare -a argv=($0)
for i in $@
do
    case $i in
        list)
            show_available_proj
            exit 0
            ;;
        -f=*|--feature=*)
            feature_mk="${i#*=}"
            ;;
        -*)
            echo "Error: unknown parameter \"$i\""
            show_usage
            exit 1
            ;;
        *)
            argv+=($i)
            ;;
    esac
done

# check configurations
board=${argv[1]}
project=${argv[2]}
output_dir=$OUT/$board/$project
mapping_var="map__${board}__${project}"
eval dsp1_proj=\${$mapping_var["dsp1"]}
eval dsp0_proj=\${$mapping_var["dsp0"]}
eval cm4_proj=\${$mapping_var["cm4"]}

# To do actions according to arguments
if [ "${argv[3]}" == "clean" ]; then
    if [ "${#argv[@]}" != "4" ]; then
        show_usage
        exit 1
    fi
    if [ ! -z "$dsp1_proj" ]; then
        clean_out $working_dir/dsp/out/$board/$dsp1_proj
    fi
    if [ ! -z "$dsp0_proj" ]; then
        clean_out $working_dir/dsp/out/$board/$dsp0_proj
    fi
    if [ ! -z "$cm4_proj" ]; then
        clean_out $working_dir/mcu/out/$board/$cm4_proj
    fi
    clean_out $OUT/$board/$project

elif [ "${argv[2]}" == "clean" ]; then
    if [ "${#argv[@]}" != "3" ]; then
        show_usage
        exit 1
    fi
    clean_out $working_dir/dsp/out/$board
    clean_out $working_dir/mcu/out/$board
    clean_out $OUT/$board
elif [ "${argv[1]}" == "clean" ]; then
    if [ "${#argv[@]}" != "2" ]; then
        show_usage
        exit 1
    fi
    clean_out $working_dir/mcu/out
    clean_out $working_dir/dsp/out
    clean_out $OUT
else
    if [ "${#argv[@]}" != "3" ]; then
        show_usage
        exit 1
    fi
    eval mapping_var_key=\${!$mapping_var[@]}
    if [ -z "$mapping_var_key" ]; then
        show_usage
        echo "Error: cannot find board=$board project=$project configurations in mapping_proj.cfg."
        exit 1
    fi

    rm -rf $output_dir/log
    mkdir -p $output_dir/log
    rm -rf $output_dir/debug
    mkdir -p $output_dir/debug
    rm -rf $output_dir/download
    mkdir -p $output_dir/download
    echo "$0 $@" > $output_dir/log/build_time.log
    echo "Start CO-Build: "`date` >> $output_dir/log/build_time.log

    # dsp1 build start
    if [ ! -z "$dsp1_proj" ]; then
        echo "cd $working_dir/dsp"
        cd $working_dir/dsp
        echo "=============================================================="
        echo "Start DSP1 Build"
        echo "=============================================================="
        #echo "Start DSP1 Build: "`date` >> $output_dir/log/build_time.log
        ./build.sh $board $dsp1_proj
        DSP1_BUILD_RESULT=$?
        #echo "End DSP1 Build: "`date` >> $output_dir/log/build_time.log
        echo "=============================================================="
        echo "End DSP1 Build"
        echo "=============================================================="
        # change back to working dir
        echo "cd $working_dir/"
        cd $working_dir/
        # copy dsp files
        copy_dsp_files $board $dsp1_proj $project
    fi

    # dsp0 build start
    if [ ! -z "$dsp0_proj" ]; then
        echo "cd $working_dir/dsp"
        cd $working_dir/dsp
        echo "=============================================================="
        echo "Start DSP0 Build"
        echo "=============================================================="
        #echo "Start DSP0 Build: "`date` >> $output_dir/log/build_time.log
        ./build.sh $board $dsp0_proj
        DSP0_BUILD_RESULT=$?
        #echo "End DSP0 Build: "`date` >> $output_dir/log/build_time.log
        echo "=============================================================="
        echo "End DSP0 Build"
        echo "=============================================================="
        # change back to working dir
        echo "cd $working_dir/"
        cd $working_dir/
        # copy dsp files
        copy_dsp_files $board $dsp0_proj $project
    fi

    # cm4 build start
    if [ ! -z "$cm4_proj" ]; then
        echo "cd $working_dir/mcu"
        cd $working_dir/mcu
        echo "=============================================================="
        echo "Start CM4 Build"
        echo "=============================================================="
        #echo "Start CM4 Build: "`date` >> $output_dir/log/build_time.log
        if [ ! -z "$feature_mk" ]; then
            ./build.sh $board $cm4_proj -f=$feature_mk bl
        else
            ./build.sh $board $cm4_proj bl
        fi
        CM4_BUILD_RESULT=$?
        #echo "End CM4 Build: "`date` >> $output_dir/log/build_time.log
        echo "=============================================================="
        echo "End CM4 Build"
        echo "=============================================================="
        # change back to working dir
        echo "cd $working_dir/"
        cd $working_dir/
        # copy cm4 files
        copy_cm4_files $board $cm4_proj $project
    fi

    # update flash_download.cfg
    flash_cfg=$output_dir/download/flash_download.cfg
    if [ -e "$flash_cfg" ]; then
        if [ ! -z "$dsp0_proj" ]; then
            sed -i "s|\bdsp0_freertos_create_thread.bin|${dsp0_proj}.bin|g" $flash_cfg
            sed -i -n '{/rom:/{x;n;{/dsp0/{x;s|^#||;p;x;tc};x;p;be}};{:c;/\bdsp0_/,+2s|^#||;}};p;bend;:e;x;p;:end' $flash_cfg
        fi
        if [ ! -z "$dsp1_proj" ]; then
            sed -i "s|\bdsp1_no_rtos_initialize_system.bin|${dsp1_proj}.bin|g" $flash_cfg
            sed -i -n '{/rom:/{x;n;{/dsp1/{x;s|^#||;p;x;tc};x;p;be}};{:c;/\bdsp1_/,+2s|^#||;}};p;bend;:e;x;p;:end' $flash_cfg
        fi
    fi

    # return code
    declare -i BUILD_RESULT=0
    echo "End   CO-Build: "`date` >> $output_dir/log/build_time.log
    if [ ! -z "$DSP1_BUILD_RESULT" ]; then
        if [ "$DSP1_BUILD_RESULT" -eq "0" ]; then
            echo "DSP1 BUILD: PASS" >> $output_dir/log/build_time.log
        else
            echo "DSP1 BUILD: FAIL" >> $output_dir/log/build_time.log
            BUILD_RESULT+=1
        fi
    fi
    if [ ! -z "$DSP0_BUILD_RESULT" ]; then
        if [ "$DSP0_BUILD_RESULT" -eq "0" ]; then
            echo "DSP0 BUILD: PASS" >> $output_dir/log/build_time.log
        else
            echo "DSP0 BUILD: FAIL" >> $output_dir/log/build_time.log
            BUILD_RESULT+=1
        fi
    fi
    if [ ! -z "$CM4_BUILD_RESULT" ]; then 
        if [ "$CM4_BUILD_RESULT" -eq "0" ]; then
            echo "CM4  BUILD: PASS" >> $output_dir/log/build_time.log
        else
            echo "CM4  BUILD: FAIL" >> $output_dir/log/build_time.log
            BUILD_RESULT+=1
        fi
    fi

    if [ "$BUILD_RESULT" -eq "0" ]; then
        echo "TOTAL CO-BUILD: PASS (return code $BUILD_RESULT)" >> $output_dir/log/build_time.log
    else
        echo "TOTAL CO-BUILD: FAIL (return code $BUILD_RESULT)" >> $output_dir/log/build_time.log
    fi

    echo "=============================================================="
    echo "Summary CO-BUILD"
    echo "=============================================================="
    cat $output_dir/log/build_time.log
    mv -f $output_dir/*.log $output_dir/log/ 2> /dev/null
    exit $BUILD_RESULT

fi

