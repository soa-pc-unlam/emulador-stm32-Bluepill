public class TransmitterThread extends Thread
{
    private ClientTelnet clientTelnet;
    private ClientBluetooth clientBluetooth;
               
        public TransmitterThread(ClientTelnet client,ClientBluetooth clientBluetooth)
        {
            try
            {

                this.clientBluetooth=clientBluetooth;               
                this.clientTelnet=client;
            }
            catch (Exception e) 
            {
                System.out.println("Error: "+ e.getMessage().toString());
            }
        }
        //metodo run del hilo, que va a entrar en una espera activa para recibir los msjs del HC05
        public void run()
        {       
            int i=0;
            String message="";     
            //el hilo secundario se queda esperando para evniar mensajes
            while (!Thread.currentThread().isInterrupted())
            {
                try 
                {     
                    //recibe los datos de Qemu
                    message = clientTelnet.receiveDataFromQemu();                

                    //le transmite al bluetooth los datos que fueron recibidos desde qemu
                    clientBluetooth.sendDataToBluetooth(message);
                    Thread.sleep(10);
                }
                catch (InterruptedException e) 
                {
                    //e.printStackTrace();
                    System.out.println("Se interrumpio transmisor");
                    break;
                }
            }
            //System.out.println("Cerrando socket transmiscion...");
            //closeConnection();
        }

        

    
}
