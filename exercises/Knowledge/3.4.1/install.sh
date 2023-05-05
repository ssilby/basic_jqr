#!/bin/bash

ENV_NAME=localenv

python3 -m pip install --user virtualenv

#create virtualenv called stacksdkenv
if [ ! -e ./${ENV_NAME} ] ; then
  python3 -m virtualenv ${ENV_NAME}
fi

#activate env
. ./${ENV_NAME}/bin/activate

echo [+] confirm env:
if [ -z "$(which python3|grep ${ENV_NAME})" ]; then
  echo [!] env setup failed!
  exit 1
fi

pip3 install -r requirements.txt

bash -i

