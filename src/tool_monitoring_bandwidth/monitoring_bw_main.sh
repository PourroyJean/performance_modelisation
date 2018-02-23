#!/bin/bash
# ------------------------------------------------------------------
#                      YAMB Profiling Tool
#
#             Yet Another Memory Bandwidth Profiling Tool
# ------------------------------------------------------------------
#
#   [Jean Pourroy]
#   [Frederic Ciesielski]
#   [Patrick Demichel]
# ------------------------------------------------------------------
# Source: https://github.com/PourroyJean/performance_modelisation
# ------------------------------------------------------------------
# ref: https://github.com/RenatGilmanov/shell-script-template/
# ------------------------------------------------------------------


#================================================================
# HEADER
#================================================================
#% SYNOPSIS
#+    ${SCRIPT_NAME}    [-hv]
#%                      --start             OPTIONS             #Standalone run: used to start the monitoring
#%                      --stop                                  #Standalone run: used to stop the monitoring
#%                      --sleep <time>      OPTIONS             #sleep <time> seconds while monitoring system wide
#%                      OPTIONS --command   command args        #Monitoring a command
#% DESCRIPTION
#%    This is a script template
#%    to start any good shell script.
#%
#% OPTIONS
#%    -o [file], --output=[file]    Set log file (default=/dev/null)
#%                                  use DEFAULT keyword to autoname file
#%                                  The default value is /dev/null.
#%    -t, --timelog                 Add timestamp to log ("+%y/%m/%d@%H:%M:%S")
#%    -x, --ignorelock              Ignore if lock file exists
#%    -h, --help                    Print this help
#%    -v, --version                 Print script information
#%
#% EXAMPLES
#%    ${SCRIPT_NAME} -o DEFAULT arg1 arg2
#%
#================================================================
#- IMPLEMENTATION
#-    version         ${SCRIPT_NAME} 0.0.5
#-    author          Jean Pourroy
#-    license         TODO
#================================================================
#  HISTORY
#     27/11/2017 : script's body, header
#
#================================================================
#  DEBUG OPTION
#    set -n  # Uncomment to check your syntax, without execution.
#    set -x  # Uncomment to debug this shell script
#
#================================================================
# END_OF_HEADER
#================================================================




trap 'error "${SCRIPT_NAME}: FATAL ERROR at $(date "+%HH%M") (${SECONDS}s): Interrupt signal intercepted! Exiting now..."
	2>&1 | tee -a ${fileLog:-/dev/null} >&2 ;
	exit 99;' INT QUIT TERM
trap 'cleanup' EXIT

#============================
#  FUNCTIONS
#============================
  #== exec_cmd function ==#
exec_cmd() {
	{
		${*}
		rc_save
	} 2>&1 | fecho CAT "${*}"
	rc_restore
	rc_assert "Command failed: ${*}"
	return $rc ;
}

  #== fecho function ==#
fecho() {
	myType=${1} ; shift ;
	[[ ${SCRIPT_TIMELOG_FLAG:-0} -ne 0 ]] && printf "$( date ${SCRIPT_TIMELOG_FORMAT} )"
	printf "[${myType%[A-Z][A-Z]}] ${*}\n"
	if [[ "${myType}" = CAT ]]; then
		if [[ ${SCRIPT_TIMELOG_FLAG:-0} -eq 0 ]]; then
			cat -un - | awk '$0="[O] "$0; fflush();' ;
		elif [[ "${GNU_AWK_FLAG}" ]]; then # fast - compatible linux
			cat -un - | awk -v tformat=${SCRIPT_TIMELOG_FORMAT#+} '$0=strftime(tformat)"[O] "$0; fflush();' ;
		else # average speed and more resource intensive- compatible unix/linux
			cat -un - | while read LINE; do \
				[[ ${OLDSECONDS:=$(( ${SECONDS}-1 ))} -lt ${SECONDS} ]] && OLDSECONDS=$(( ${SECONDS}+1 )) \
				&& TSTAMP="$( date ${SCRIPT_TIMELOG_FORMAT} )"; printf "${TSTAMP}[O] ${LINE}\n"; \
			done
		fi
	fi
}

  #== custom function ==#
check_cre_file() {
	myFile=${1}
	[[ "${myFile}" = "/dev/null" ]] && return 0
	[[ -e ${myFile} ]] && error "${SCRIPT_NAME}: ${myFile}: File already exists" && return 1
	touch ${myFile} 2>&1 1>/dev/null
	[[ $? -ne 0 ]] && error "${SCRIPT_NAME}: ${myFile}: Cannot create file" && return 2
	rm -f ${myFile} 2>&1 1>/dev/null
	[[ $? -ne 0 ]] && error "${SCRIPT_NAME}: ${myFile}: Cannot delete file" && return 3
	return 0
}

#============================
#  ALIAS AND FUNCTIONS
#============================

  #== error management function ==#
info() { fecho INF "${*}"; }
warning() { fecho WRN "WARNING: ${*}" 1>&2; countWrn=$(( ${countWrn} + 1 )); }
error() { fecho ERR "ERROR: ${*}" 1>&2; countErr=$(( ${countErr} + 1 )); }
cleanup() { [[ -e "${fileRC}" ]] && rm ${fileRC}; [[ -e "${fileLock}" ]] && [[ "$( head -1 ${fileLock} )" = "${EXEC_ID}" ]] && rm ${fileLock}; }
scriptfinish() { [[ $rc -eq 0 ]] && endType="INF" || endType="ERR";
	fecho ${endType} "${SCRIPT_NAME} finished at $(date "+%HH%M") (Time=${SECONDS}s, Error=${countErr}, Warning=${countWrn}, RC=$rc).";
	exit $rc ; }

  #== usage function ==#
usage() { printf "Usage: "; scriptinfo usg ; }
usagefull() { scriptinfo ful ; }
scriptinfo() { headFilter="^#-"
	[[ "$1" = "usg" ]] && headFilter="^#+"
	[[ "$1" = "ful" ]] && headFilter="^#[%+]"
	[[ "$1" = "ver" ]] && headFilter="^#-"
	head -${SCRIPT_HEADSIZE:-99} ${0} | grep -e "${headFilter}" | sed -e "s/${headFilter}//g" -e "s/\${SCRIPT_NAME}/${SCRIPT_NAME}/g"; }

  #== inter program return code function ==#
rc_save() { rc=$? && echo $rc > ${fileRC} ; }
rc_restore() { [[ -r "${fileRC}" ]] && rc=$(cat ${fileRC}) ; }
rc_assert() { [[ $rc -ne 0 ]] && error "${*} (RC=$rc)"; }

#============================
#  FILES AND VARIABLES
#============================

  #== general variables ==#
SCRIPT_HEADSIZE=$(grep -sn "^# END_OF_HEADER" ${0} | head -1 | cut -f1 -d:)
SCRIPT_ID="$(scriptinfo | grep script_id | tr -s ' ' | cut -d' ' -f3)"
SCRIPT_NAME="$(basename ${0})" # scriptname without path
SCRIPT_UNIQ="${SCRIPT_NAME%.*}${SCRIPT_ID}.${HOSTNAME%%.*}"
SCRIPT_UNIQ_DATED="${SCRIPT_UNIQ}.$(date "+%y%m%d%H%M%S").${$}"
SCRIPT_DIR="$( cd $(dirname "$0") && pwd )" # script directory
SCRIPT_PYTHON="$SCRIPT_DIR""/format_log.py"
DIR_TMP="/nfs/pourroy/tmp"                                  # Make sure temporary folder is RW
YAMB_ANNOTATE_LOG_FILE="${DIR_TMP}/yamb_annotate_log_file"  # Make sure this file is the same as the one used in YAMB (code_annotation.h file)


SCRIPT_TIMELOG_FLAG=0
SCRIPT_TIMELOG_FORMAT="+%y/%m/%d@%H:%M:%S"

HOSTNAME="$(hostname)"
FULL_COMMAND="${0} $*"
EXEC_DATE=$(date "+%y%m%d%H%M%S")
EXEC_ID=${$}
GNU_AWK_FLAG="$(awk --version 2>/dev/null | head -1 | grep GNU)"




fileRC="${DIR_TMP}/${SCRIPT_UNIQ_DATED}.tmp.rc";
fileLock="${DIR_TMP}/${SCRIPT_UNIQ}.lock"

#Trace are generated in the current folder
USER_PATH=`pwd`
filebase="$USER_PATH/log_$SCRIPT_UNIQ_DATED"
fileLog="${filebase}.perf.log"
fileAnnotate="${filebase}.annotate"

_PYTHON_IMAGE_PATH=""

SLEEP_TIME=0

#== Annotation ==#






rc=0;

countErr=0;
countWrn=0;

  #== option variables ==#
flagOptAction=0
flagOptErr=0
flagOptLog=0
flagOptTimeLog=0
flagOptImage=0
flagOptIgnoreLock=0
flagOptImageRequired=0
flagOptImageWithNoPath=0
flagActionStart=0
flagActionCommand=0
flagActionSleep=0
flagActionStop=0


#============================
#  PARSE OPTIONS WITH GETOPTS
#============================

  #== set short options ==#
SCRIPT_OPTS=':a:o:i:w:x:thvyz-:'
nb_arg=$#

  #== set long options associated with short one ==#
typeset -A ARRAY_OPTS
ARRAY_OPTS=(
	[timelog]=t
#	[ignorelock]=x
	[output]=o
	[image]=i
	[help]=h
	[man]=h

    [sleep]=w
	[command]=x
	[start]=y
	[stop]=z
)

  #== parse options ==#
while getopts ${SCRIPT_OPTS} OPTION ; do
	#== translate long options to short ==#
	if [[ "x$OPTION" == "x-" ]]; then
		LONG_OPTION=$OPTARG
		LONG_OPTARG=$(echo $LONG_OPTION | grep "=" | cut -d'=' -f2)
		LONG_OPTIND=-1
		[[ "x$LONG_OPTARG" = "x" ]] && LONG_OPTIND=$OPTIND || LONG_OPTION=$(echo $OPTARG | cut -d'=' -f1)
		[[ $LONG_OPTIND -ne -1 ]] && eval LONG_OPTARG="\$$LONG_OPTIND"
		OPTION=${ARRAY_OPTS[$LONG_OPTION]}
		[[ "x$OPTION" = "x" ]] &&  OPTION="?" OPTARG="-$LONG_OPTION"

		if [[ $( echo "${SCRIPT_OPTS}" | grep -c "${OPTION}:" ) -eq 1 ]]; then
			if [[ "x${LONG_OPTARG}" = "x" ]] || [[ "${LONG_OPTARG}" = -* ]]; then
				OPTION=":" OPTARG="-$LONG_OPTION"
			else
				OPTARG="$LONG_OPTARG";
				if [[ $LONG_OPTIND -ne -1 ]]; then
					[[ $OPTIND -le $Optnum ]] && OPTIND=$(( $OPTIND+1 ))
					shift $OPTIND
					OPTIND=1
				fi
			fi
		fi
	fi

	#== options follow by another option instead of argument ==#
	if [[ "x${OPTION}" != "x:" ]] && [[ "x${OPTION}" != "x?" ]] && [[ "${OPTARG}" = -* ]]; then
		OPTARG="$OPTION" OPTION=":"
	fi

	#== manage options ==#
	case "$OPTION" in
        y )
#            echo "START"
            flagActionStart=1

        ;;

        z )
#            echo "STOP"
            flagActionStop=1
        ;;

        x )
#            echo "COMMAND"
            flagActionCommand=1

        ;;

        w )
#            echo "SLEEP"
            if [[ $OPTARG =~ ^-?[0-9]+$ ]]; then
                SLEEP_TIME=$OPTARG
            else
                error "Sleep has to be used with a integer (number of second to sleep)"
                exit -1
            fi
            flagActionSleep=1

        ;;

		o ) filebase="$USER_PATH/${OPTARG}"
			[[ "${OPTARG}" = *"DEFAULT" ]] && filebase="$( echo ${OPTARG} | sed -e "s/DEFAULT/${SCRIPT_UNIQ_DATED}.log/g" )"
            if [[ "${OPTARG:0:1}" == / || "${OPTARG:0:2}" == ~[/a-z] ]]
            then
                filebase=${OPTARG}
            else
                filebase="$USER_PATH/${OPTARG}"
            fi

            fileLog=${filebase}.perf.log
            fileAnnotate=${filebase}.annotate
			flagOptLog=1
		;;

		t ) flagOptTimeLog=1
			SCRIPT_TIMELOG_FLAG=1
		;;
		i ) flagOptImageRequired=1
            if [[ "${OPTARG:0:1}" == / || "${OPTARG:0:2}" == ~[/a-z] ]]
            then
                _PYTHON_IMAGE_PATH=${OPTARG}
            else
                _PYTHON_IMAGE_PATH="$USER_PATH/${OPTARG}"
            fi
            flagOptImageRequired=1

		;;

		x ) flagOptIgnoreLock=1
		;;

		h ) usagefull
			exit 0
		;;

		v ) scriptinfo
			exit 0
		;;

		: )
		    if  [ "$OPTARG" == "i" ] ; then
                flagOptImageRequired=1
		    else
		        error "${SCRIPT_NAME}: -$OPTARG: option requires an argument"
			    flagOptErr=1
            fi
		;;

		? ) error "${SCRIPT_NAME}: -$OPTARG: unknown option"
			flagOptErr=1
		;;
	esac
done
shift $((${OPTIND} - 1)) ## shift options




#=======================================================================================================================
#=======================================================================================================================
#  MAIN SCRIPT
#=======================================================================================================================
#=======================================================================================================================

_TOKEN='JANNOT'
_PID=0

_EVENT_BW_READ="uncore_imc_0/cas_count_read/,uncore_imc_1/cas_count_read/,uncore_imc_2/cas_count_read/,uncore_imc_3/cas_count_read/,uncore_imc_4/cas_count_read/,uncore_imc_5/cas_count_read/"
_EVENT_BW_WRITE="uncore_imc_0/cas_count_write/,uncore_imc_1/cas_count_write/,uncore_imc_2/cas_count_write/,uncore_imc_3/cas_count_write/,uncore_imc_4/cas_count_write/,uncore_imc_5/cas_count_write/"
_EVENTS="-e ${_EVENT_BW_WRITE},${_EVENT_BW_READ}"
_PERF_PREFIX="perf stat -a -x,  "
_PERF_RATE=100
_PERF_LOG="-o $fileLog"
_PERF_CMD="$_PERF_PREFIX $_PERF_LOG $_EVENTS -I $_PERF_RATE"


_CONFIGURATION_FILE="${DIR_TMP}/yamb_configuration_file"
_IS_DISPLAY_AVAILABLE=false
_PYTHON_CMD="python $SCRIPT_PYTHON"
_PYTHON_IMAGE_CMD=""
_PYTHON_ANNOTATE_CMD=""
_time_start=0
_time_stop=0

f_execute_python (){
    _PYTHON_CMD="$SCRIPT_PYTHON --data $fileLog $_PYTHON_IMAGE_CMD $_PYTHON_ANNOTATE_CMD"
    info "Python execution: $_PYTHON_CMD"
    python $_PYTHON_CMD
}

f_optimise_annotation_file () {
    # We are looking from annotation that append during the monitoring  ($_time_start and $_time_stop)
    index_start=`awk '{a[NR]=$0}END{for(i=0;i< NR;i++) if (a[i] > '$_time_start') { print i ; break }  }'  $fileAnnotate `
    index_stop=` awk '{a[NR]=$0}END{for(i=NR;i>=1;i--) if (a[i] < '$_time_stop')  { print i ; break }  }'  $fileAnnotate `

    if [ -n "$index_start" ] && [ -n "$index_stop" ]; then
        info "-- OPTIMISATION of Annotation file -- We only keep lines between $index_start and $index_stop"
        sed -n "$index_start,$index_stop p;$index_stop q" $fileAnnotate > ${fileAnnotate}.bis
        mv ${fileAnnotate}.bis $fileAnnotate
    fi
}

f_start_monitoring (){
    #Save the log's path in a temporary file ${DIR_TMP}/yamb_log_file and remove previous run's information
    echo "$filebase  $(date +%s%N | cut -b1-13)" > $_CONFIGURATION_FILE
    echo "bash -c " $_PERF_CMD " &"
    bash -c " $_PERF_CMD " &
}


f_stop_monitoring (){
    _time_stop=`date +%s%N | cut -b1-13`
    #Recover information from the running session
    _PID=`ps aux |  grep "perf stat -a" | grep -v 'grep' | head -n 1  | awk '{ print $2}'`
    while [ ! -z $_PID ] ; do
        info    "YAMB process found and killed (PID=$_PID)"
        kill -kill $_PID
        _PID=`ps aux |  grep "perf stat -a" | grep -v 'grep' | head -n 1  | awk '{ print $2}'`
    done

    #Annotation file: add the start date in the first line of the annotation file
    mv $YAMB_ANNOTATE_LOG_FILE $fileAnnotate  2> /dev/null

    #-- PYTHON: annotation
    if [  -f $fileAnnotate ]; then
        echo "An annotation file was found: $fileAnnotate"
        _time_start=$(cat $_CONFIGURATION_FILE | awk '{print $2}' )
        sed -i " 1 s/.*/& $_time_start/" $fileLog
        _PYTHON_ANNOTATE_CMD=" --annotate ${fileAnnotate}"
        f_optimise_annotation_file
    fi

    echo "OFF" > $_CONFIGURATION_FILE
}





f_sanity_check (){
    if [ "$nb_arg" -ne 1 ] && [[ ${flagActionStop} -eq 1 ]]; then
        echo "stop has to be used without other options"
        exit -1
    fi

     #== Check files ==#
    check_cre_file "${YAMB_ANNOTATE_LOG_FILE}_test"  || (echo "Check the YAMB_ANNOTATE_LOG_FILE variable " && exit 3)

}

f_configure (){

    #User wants to print the graph into a PNG file: no screen output
    if [[ $flagOptImageRequired -eq 1 ]]; then
        info    "Python output to png file"
        _PYTHON_IMAGE_CMD="-i $_PYTHON_IMAGE_PATH"
        _IS_DISPLAY_AVAILABLE=false
    else
    #Is no option set: check if the display is available
        if `python -c "import matplotlib.pyplot as plt;plt.figure()" 2&> /dev/null`  ; then
            info "Python will show the graph on the screen"
            _IS_DISPLAY_AVAILABLE=true
        else
    #If the display is not available: generate a picture with the same name as the log file
            warning "No display has been found (Hint: try to < ssh -X > the node)"
            info    "Python output will be redirected in a png file"
            _PYTHON_IMAGE_CMD="-i "
            _IS_DISPLAY_AVAILABLE=false
        fi
    fi

    if [[ ${flagActionStop} -eq 1 ]]; then
        filebase=$(cat $_CONFIGURATION_FILE | awk '{print $1}' )
        fileLog=${filebase}.perf.log
        fileAnnotate=${filebase}.annotate
    fi



}




f_print_configuration (){
    echo ""
    info "  _ SCRIPT PATH:          $SCRIPT_DIR/$SCRIPT_NAME"
    info "  _ SCRIPT DIR:           $SCRIPT_DIR"
    info "  _ PRINT ON SCREEN:      $_IS_DISPLAY_AVAILABLE"
    info "  _ LOG FILE PATH:        $fileLog"
    info "  _ LOG ANNOTATE PATH:    $fileAnnotate"
    info "  _ SAMPLE (every N ms):  $_PERF_RATE"
    info "  _ COMMAND PERF:         $_PERF_CMD"
    info "  _ PYTHON PARSER:        $SCRIPT_PYTHON"
    echo ""
}

f_check_start (){
    if [[ -e ${_CONFIGURATION_FILE} ]] ; then
        if ! grep -q OFF "$_CONFIGURATION_FILE" ; then
            error "YAMB is already running"
            exit -1
        fi
    fi
}


[ $flagOptErr -eq 1 ] && usage 1>&2 && exit 1 ## print usage if option error and exit




#----------
#   INIT  -
#----------
info "---- CONFIGURATION ----"

f_sanity_check $*

f_configure

f_print_configuration




#----------
#  START  -
#   - Standalone
#   - For a command
#   - For a time
#----------
if [[ ${flagActionStart} -eq 1 ]]; then
    info "-> ACTION START: standalone"
    rm $fileLog  2&> /dev/null
    rm $fileAnnotate 2&> /dev/null

    f_check_start
    f_start_monitoring

elif [[ ${flagActionCommand} -eq 1 ]]; then
#elif [ -x "$(command -v $1)" ]; then
    info "-> ACTION START: monitor the following command: $1"
    f_check_start


elif [[ ${flagActionSleep} -eq 1 ]]; then
#elif [[ $1 =~ ^-?[0-9]+$ ]]; then
    info "_START monitoring for $SLEEP_TIME seconds"
    f_check_start


#----------
#   STOP  -
#----------
elif [[ ${flagActionStop} -eq 1 ]]; then
    info "ACTION _STOP "
    is_yamb_running=$(cat $_CONFIGURATION_FILE)

    if grep -q OFF "$_CONFIGURATION_FILE" ; then
        error "YAMB was not running..."
        exit -1
    fi

    f_stop_monitoring
    f_execute_python




else
    error "${SCRIPT_NAME}: Wrong argument" && usage 1>&2 && exit 2
fi


exit



  #== Create files ==#
[[ "${fileLog}" != "/dev/null" ]] && touch ${fileLog} && fileLog="$( cd $(dirname "${fileLog}") && pwd )"/"$(basename ${fileLog})"
[[ ! -e ${fileLock} ]] && echo "${EXEC_ID}" > ${fileLock}


  #== Main part ==#
  #===============#
{ trap 'kill -TERM ${$}; exit 99;' TERM

info "${SCRIPT_NAME}: start $(date "+%y/%m/%d@%H:%M:%S") with process id ${EXEC_ID} by ${USER}@${HOSTNAME}:${PWD}"\
	$( [[ ${flagOptLog} -eq 1 ]] && echo " (LOG: ${fileLog})" || echo " (NOLOG)" );

  #== start your program here ==#
exec_cmd "ls -lrt $*"
info "Sleeping for 2 seconds ..." && sleep 1
  #== end   your program here ==#

scriptfinish ; } 2>&1 | tee ${fileLog}

  #== End ==#
  #=========#
rc_restore
exit $rc

