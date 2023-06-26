

public class ReceiverThread extends Thread
{
 
    private ClientTelnet clientTelnet;
    private ClientBluetooth clientBluetooth;

        public ReceiverThread(ClientTelnet clientTelnet,ClientBluetooth clientBluetooth)
        {
            try
            {
                this.clientTelnet=clientTelnet;
                this.clientBluetooth=clientBluetooth;
            }
            catch (Exception e) 
            {
                System.out.println(e.getMessage().toString());
            }
        }
    
        //metodo run del hilo, que va a entrar en una espera activa para recibir los msjs del HC05
        public void run()
        {
            String readMessage;
            //el hilo secundario se queda esperando mensajes del HC05
            while (!Thread.currentThread().isInterrupted())
            {
                //recive datos del bluetooth
                readMessage=clientBluetooth.receiveDataFromBluetooth();
                
                if(readMessage.equals(null))
                    break;
                //le transmite a qemu los datos que fueron recibidos del bluetooth 
                clientTelnet.sendDataToQemu(readMessage);

                System.out.println(readMessage);
                
            }

           closeConnection();
            
        }


        public void closeConnection()
        {
                
            //cierro el socket del cliente telnet
            clientTelnet.socketClose();
            clientBluetooth.socketClose();            
        
            System.out.println("Cerrando socket recepecion...");
        
        }

}
