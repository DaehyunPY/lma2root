#!/bin/bash
MACHINE="2016-box"
IMAGE="daehyunpy/lma2root:latest"
eval "$(docker-machine env --shell bash $MACHINE)"

if [ $# -le 0 ]
then
    docker run --rm $IMAGE powershell.exe lma2root.exe --help
else
    echo "Get-Content \"$*\" | lma2root.exe -b" > run.ps1
    docker run --rm --interactive --tty \
        --volume "C:$(pwd):C:/work" \
        $IMAGE powershell.exe .\\run.ps1
    rm run.ps1
fi
