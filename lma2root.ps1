Set-Variable IMAGE "daehyunpy/lma2root:latest"

If ( $args.Count -le 0 ) {
    docker run --rm $IMAGE powershell.exe lma2root.exe --help
} Else {
    "Get-Content ""$args"" | lma2root.exe -b" | Out-File run.ps1
    docker run --rm --interactive --tty `
        --volume "$(Get-Location):C:\work" `
        $IMAGE powershell.exe .\run.ps1
    Remove-Item run.ps1
}
