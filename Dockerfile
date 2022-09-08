#se descarga la version de Ubuntu 20.04
from ubuntu:20.04

ARG DEBIAN_FRONTEND='noninteractive'

#instalo los paquetes necesario de Ubuntu
RUN  apt-get update && apt-get install -y --no-install-recommends \
     apt-utils \
     gcc-arm-none-eabi \
     libnewlib-arm-none-eabi \
     findutils \
     gcc \
     git \
     python2.7 \
     pkgconf \ 
     libglib2.0-dev \
     libfdt-dev \
     libpixman-1-dev \
     zlib1g-dev \
     make \
     pkgconf \
     openssh-client \
     psmisc \
     net-tools \
     iputils-ping \
     ca-certificates \
     ssh \
     && rm -rf /var/lib/apt/lists/* #se borra la cache de apt para liberar espacio
     
     
#descargo del repositorio de SOA la imagen de QEMU  que hizo beckus, que fue modifcada para poder ejecutar la placas Bluepill
RUN cd / &&\
    git clone https://github.com/soaunlam2021/emulador-stm32-Bluepill.git

#Compile and create image of qemu
RUN cd /emulador-stm32-Bluepill/qemu/qemu_stm32 &&\
    chmod 555 configure &&\
   ./configure --extra-cflags="-w" --enable-debug --target-list="arm-softmmu" --python=/usr/bin/python2.7 &&\
    make

#compilo todos los ejemplo para las ejecutar en las disintas placas
RUN cd /emulador-stm32-Bluepill/stm32_demos/STM32F103C8T6/demos &&\
    make

#se modifica el archivo sshd_config, para poder acceder por sshs al Docker
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/g' /etc/ssh/sshd_config &&\
    sed -i 's/#PasswordAuthentication/PasswordAuthentication/g' /etc/ssh/sshd_config

#se establece el password root
RUN echo 'root:1234' | chpasswd

#agrego comandos que se ejecutaran cada vez que se inicia el contenedor
RUN echo alias qemu-system-arm="/emulador-stm32-Bluepill/qemu/qemu_stm32/arm-softmmu/qemu-system-arm" >> /root/.bashrc &&\
    echo echo "Comando qemu-system-arm creado." >> /root/.bashrc &&\
    echo service ssh restart >> /root/.bashrc &&\
    echo echo "Servicio ssh incializado..." >> /root/.bashrc 


