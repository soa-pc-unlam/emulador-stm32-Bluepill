# SOA-Emulador-STM32-Bluepill

El código fuente de este repositorio pertenece al proyecto denominado "Contenedores de ambientes simulados de sistemas embebidos para Internet de las Cosas" que se esta llevando a cabo en la Universidad Nacional de La Matanza

Este se divide en los siguientes directorios:

- **Docker:** Directorio que almacena el código fuente del contenedor que contiene el emulador qemu configurado para simular la placa Bluepill y poder comunicarla a través de Bluetooth
- **Gateway-windows:** En este direcrtorio se alamcena el código fuente en Java que hace de intermediario entre el mulador Qemu en Docker y un Smarthpone, a través de Bluetooth en Windows.
- **Apps-Android:** En este direcrtorio contiene el código fuente y configuraciones de las aplicaciones para Android, que se comunicaran a traves de Bluetooth con el emulador Qemu.