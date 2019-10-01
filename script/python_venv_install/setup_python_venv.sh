#!/usr/bin/env bash


#Use latest python version available here: /apps/python/miniconda3/bin/
echo "# - - - # Creating virtual env..."
python3 -m venv venv_perf && echo "env created in venv_perf" || (echo "not able to create the venv" && exit -1)
echo "# - - - # Activating virtual env..."
source venv_perf/bin/activate && echo "the venv has been activated" || (echo "not able to activate the venv" && exit -2)
echo "# - - - # Installing package from requiremnts.txt..."
pip install -r requirements.txt && echo "all requirements installed" || (echo "not able to install all requirements" && exit -3)

echo "# - - - # You can now activate the virtual env by running the following command : "
echo "# - - - #     source venv_perf/bin/activate"
echo "# - - - # To leave the virtual env use:"
echo "# - - - #     deactivate"

