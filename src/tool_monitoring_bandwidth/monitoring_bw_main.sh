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
#+    ${SCRIPT_NAME} [-hv] [-o[file]] args ...
#%
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
SCRIPT_DIR_TEMP="/tmp" # Make sure temporary folder is RW
USER_PATH=`pwd`

SCRIPT_TIMELOG_FLAG=0
SCRIPT_TIMELOG_FORMAT="+%y/%m/%d@%H:%M:%S"

HOSTNAME="$(hostname)"
FULL_COMMAND="${0} $*"
EXEC_DATE=$(date "+%y%m%d%H%M%S")
EXEC_ID=${$}
GNU_AWK_FLAG="$(awk --version 2>/dev/null | head -1 | grep GNU)"

fileRC="${SCRIPT_DIR_TEMP}/${SCRIPT_UNIQ_DATED}.tmp.rc";
fileLock="${SCRIPT_DIR_TEMP}/${SCRIPT_UNIQ}.lock"
fileLog="$USER_PATH/log_$SCRIPT_UNIQ_DATED"

rc=0;

countErr=0;
countWrn=0;

  #== option variables ==#
flagOptErr=0
flagOptLog=0
flagOptTimeLog=0
flagOptIgnoreLock=0

#============================
#  PARSE OPTIONS WITH GETOPTS
#============================

  #== set short options ==#
SCRIPT_OPTS=':o:txhv-:'

  #== set long options associated with short one ==#
typeset -A ARRAY_OPTS
ARRAY_OPTS=(
	[timelog]=t
	[ignorelock]=x
	[output]=o
	[help]=h
	[man]=h
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
		o ) fileLog="$USER_PATH/${OPTARG}"
			[[ "${OPTARG}" = *"DEFAULT" ]] && fileLog="$( echo ${OPTARG} | sed -e "s/DEFAULT/${SCRIPT_UNIQ_DATED}.log/g" )"
            if [[ "${OPTARG:0:1}" == / || "${OPTARG:0:2}" == ~[/a-z] ]]
            then
                fileLog=${OPTARG}
            else
                fileLog="$USER_PATH/${OPTARG}"
            fi
			flagOptLog=1
#			echo "FILE LOG option: $fileLog"
		;;

		t ) flagOptTimeLog=1
			SCRIPT_TIMELOG_FLAG=1
		;;

		x ) flagOptIgnoreLock=1
		;;

		h ) usagefull
			exit 0
		;;

		v ) scriptinfo
			exit 0
		;;

		: ) error "${SCRIPT_NAME}: -$OPTARG: option requires an argument"
			flagOptErr=1
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
_CMD_PERF="perf stat -a -x,  "
_LOG="-o $fileLog"
_EVENT_BW_READ="uncore_imc_0/cas_count_read/,uncore_imc_1/cas_count_read/,uncore_imc_2/cas_count_read/,uncore_imc_3/cas_count_read/,uncore_imc_4/cas_count_read/,uncore_imc_5/cas_count_read/"
_EVENT_BW_WRITE="uncore_imc_0/cas_count_write/,uncore_imc_1/cas_count_write/,uncore_imc_2/cas_count_write/,uncore_imc_3/cas_count_write/,uncore_imc_4/cas_count_write/,uncore_imc_5/cas_count_write/"
_EVENTS="-e ${_EVENT_BW_WRITE},${_EVENT_BW_READ}"
_PERF_RATE=100
_LOG_FILE_PATH_SAVE="/tmp/yamb_log_file"
_IS_DISPLAY_AVAILABLE=false



#Param 1: path of the log file
f_execute_python (){
    if $_IS_DISPLAY_AVAILABLE ; then
        info "Python script will read data from : $fileLog"
        python $SCRIPT_PYTHON $fileLog

    else
        warning "No display has been found, you should try to < ssh -X > the node"
    fi

}

f_start_monitoring (){
    echo $fileLog > $_LOG_FILE_PATH_SAVE
    bash -c "  $_CMD_PERF $_LOG $_EVENTS -I $_PERF_RATE" &

}


f_stop_monitoring (){
    $fileLog=$(cat $_LOG_FILE_PATH_SAVE)
    echo "OFF" > $_LOG_FILE_PATH_SAVE
    _PID=`ps aux |  grep "perf stat -a" | grep -v 'grep' | head -n 1  | awk '{ print $2}'`
    while [ ! -z $_PID ] ; do
        info    "YAMB process found and killed (PID=$_PID)"
        kill -kill $_PID
        _PID=`ps aux |  grep "perf stat -a" | grep -v 'grep' | head -n 1  | awk '{ print $2}'`
    done

    f_execute_python
}

f_configure (){
    if ! `python -c "import matplotlib.pyplot as plt;plt.figure()" 2&> /dev/null`  ; then
        _IS_DISPLAY_AVAILABLE=false
    else
        _IS_DISPLAY_AVAILABLE=true
    fi
}

f_print_configuration (){
    echo ""
    info "  _ SCRIPT PATH:          $SCRIPT_DIR/$SCRIPT_NAME"
    info "  _ SCRIPT DIR:           $SCRIPT_DIR"
    info "  _ PYTHON PARSER:        $SCRIPT_PYTHON"
    info "  _ LOG FILE PATH:        $fileLog"
    info "  _ IS DISPLAY AVAILABLE: $_IS_DISPLAY_AVAILABLE"
    info "  _ COMMAND PERF:         $_CMD_PERF $_LOG $_EVENTS $_PERF_RATE"
    info "  _ SAMPLE (every N ms):  $_PERF_RATE"
    echo ""
}

f_check_start (){
   if ! grep -q OFF "$_LOG_FILE_PATH_SAVE" ; then
        error "YAMB is already launched"
        exit -1
    fi


}



[ $flagOptErr -eq 1 ] && usage 1>&2 && exit 1 ## print usage if option error and exit

  #== Check/Set arguments ==#
[[ $# -gt 1 ]] && error "${SCRIPT_NAME}: Too many arguments" && usage 1>&2 && exit 2



info "---- CONFIGURATION ----"
f_configure
f_print_configuration



#re='^[0-9]+$'

#----------
#  START  -
#----------
if [ "$1" == "start" ]; then
    info "-> ACTION START: standalone"
    f_check_start
    f_start_monitoring


elif [ -x "$(command -v $1)" ]; then
    info "-> ACTION START: monitor the following command: $1"
    f_check_start

elif [[ $1 =~ ^-?[0-9]+$ ]]; then
    info "_START monitoring for $1 seconds"
    f_check_start


#----------
#   STOP  -
#----------
elif  [ "$1" == "stop" ]; then
    info "ACTION _STOP "
    is_yamb_running=$(cat $_LOG_FILE_PATH_SAVE)

    if grep -q OFF "$_LOG_FILE_PATH_SAVE" ; then
        error "YAMB was not launched..."
        exit -1
    fi

    f_stop_monitoring
else
    error "${SCRIPT_NAME}: Wrong argument" && usage 1>&2 && exit 2
fi




exit


  #== Check files ==#
check_cre_file ${fileRC}  || exit 3
check_cre_file ${fileLog} || exit 3
if [[ ${flagOptIgnoreLock} -eq 0 ]]; then
	[[ -e ${fileLock} ]] && error "${SCRIPT_NAME}: ${fileLock}: lock file detected" && exit 4
	check_cre_file ${fileLock} || exit 4
fi

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



