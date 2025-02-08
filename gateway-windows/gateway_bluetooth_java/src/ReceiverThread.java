

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
        public void run() {
            try {
                while (!Thread.currentThread().isInterrupted()) {
                    String readMessage = clientBluetooth.receiveDataFromBluetooth();
                    if (readMessage == null) break;
                    clientTelnet.sendDataToQemu(readMessage);
                }
            } catch (Exception e) {
                System.out.println("Error en ReceiverThread: " + e.getMessage());
            } finally {
                closeConnection();
            }
        }
        


        public void closeConnection()
        {
                
            //cierro el socket del cliente telnet
            clientTelnet.socketClose();
            clientBluetooth.socketClose();            
        
            System.out.println("Closing socket recpeption...");
        
        }

}
