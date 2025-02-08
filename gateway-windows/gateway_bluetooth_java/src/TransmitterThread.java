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
        public void run() {       
            try {
                while (!Thread.currentThread().isInterrupted()) {
                    String message = clientTelnet.receiveDataFromQemu();
                    
                    //si hubo un problema en la recepcion del mensaje salgo del while
                    if (message==null)
                        break;
                    
                    clientBluetooth.sendDataToBluetooth(message);
                    Thread.sleep(10);
                }
            } catch (InterruptedException e) {
                System.out.println("Transmitter interrupted");
                Thread.currentThread().interrupt();
            } finally {
                //closeConnection();
            }
        }
        
        

    
}
