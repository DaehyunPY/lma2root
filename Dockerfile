FROM microsoft/windowsservercore:ltsc2016

LABEL maintainer="Daehyun You <daehyun@dc.tohoku.ac.jp>"

SHELL [ "powershell.exe", "-ExecutionPolicy", "Bypass", "-Command" ]

WORKDIR /
ADD https://aka.ms/vs/15/release/vs_buildtools.exe vs_buildtools.exe
RUN C:\\vs_buildtools.exe --quiet --wait --norestart --nocache \
        --installPath C:\\BuildTools \
        --add Microsoft.VisualStudio.Workload.MSBuildTools \
        --add Microsoft.VisualStudio.Workload.VCTools \
        --add Microsoft.VisualStudio.Component.Windows10SDK.17134 | Out-Null; \
    Remove-Item .\\vs_buildtools.exe

ADD https://root.cern.ch/download/root_v5.34.36.win32.vc12.zip /root.zip
RUN Expand-Archive root.zip C:\\; \
    Remove-Item .\\root.zip
ENV ROOTSYS=C:\\root \
    PATH=C:\\root\\bin;$PATH

WORKDIR /app
COPY lma2root /app/
RUN C:\\BuildTools\\MSBuild\\15.0\\Bin\\MSBuild.exe .\\lma2root.sln \
    /m /p:Configuration=Release /p:Platform=Win32
ENV PATH=C:\\app\\Binaries;$PATH

WORKDIR /work
CMD [ "powershell.exe", "-NoLogo", "-ExecutionPolicy", "Bypass" ]
