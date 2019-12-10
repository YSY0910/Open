using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
#if REAL_LATTE
using LattePanda.Firmata; //LattePanda 보드 사용시에만..
#endif
using Npgsql; //use Npgsql
using System.Net;
using System.Net.Mail;
using System.Diagnostics; //for use Process

namespace LatteTest
{
    public static class GRun{
       public static String secureLoginStr = "Host=localhost;Port=9603;Username=postgres;Password=admin0;Database=xsecure";
       public static Sensor_data mySensor = new Sensor_data();
       public static DImagePath myDImg = new DImagePath();
       public static MySettings mySet = new MySettings();
       public static HostStatus hostStatus = new HostStatus();

       public static void InitGRun(){
            mySensor.Clear();
            myDImg.Clear();
            mySet.Clear();
            hostStatus.Clear();
        }
    }

    //################################################
    public class Sensor_data
    {
        public String sTem;
        public String sGas;
        public String sFlar;

        public Sensor_data(){
            Clear();
        }

        public void Clear(){
            //sTem = String.Empty; sGas = String.Empty; sFlar = String.Empty;
            sTem = "0"; sGas = "0"; sFlar = "0";
        }

        public bool Insert_SensorValue(){            
            using ( var conn = new NpgsqlConnection(GRun.secureLoginStr))
            {
                try{
                    conn.Open();//###
#if true
                    using (var command = new NpgsqlCommand()) {
                        command.Connection = conn;
                        //string sqlQuery = "INSERT INTO s_sensors(s_tem, VALUES (s_tem,s_gas,s_flar)
                        command.CommandText = "INSERT INTO s_sensors(s_tem,s_gas,s_flar) VALUES(@s_tem,@s_gas,@s_flar)";                        
                        command.Parameters.AddWithValue("s_tem", sTem);
                        command.Parameters.AddWithValue("s_gas", sGas);
                        command.Parameters.AddWithValue("s_flar", sFlar);
                        Console.WriteLine($"{command.CommandText} \r\n{sTem},{sGas},{sFlar}");
                        command.ExecuteNonQuery();
                    }  
#endif                    
                }catch ( Exception ex){
                    Console.WriteLine($"[Error.s_sensors]:{ex.Message}");
                }finally{
                    conn.Close();//###
                }                
            }

            return true;
        }
    }

    //################################################
    public class DImagePath
    {
        public String sPath;
        public String sFName;

        public DImagePath(){
            Clear();
        }

        public void Clear(){
            sPath = String.Empty; sFName = String.Empty;
        }

        public bool Read_DImagePath(){
            using ( var conn = new NpgsqlConnection(GRun.secureLoginStr))
            {
                try{
                    conn.Open();//###

                    using (var command = new NpgsqlCommand()) {
                        command.Connection = conn;
                        command.CommandText = "SELECT spath,dimg FROM s_odimages ORDER BY index DESC LIMIT 1";
                        using(var reader = command.ExecuteReader())
                        {
                            while(reader.Read()){
                                sPath = reader["spath"].ToString();
                                sFName = reader["dimg"].ToString();
                                //sMAccount = reader["smail_addr"].ToString();
                                //Console.WriteLine($"[read dimage info]: {sPath}//{sFName}");
                            }
                        }
                    }
                }catch ( Exception ex){
                    Console.WriteLine($"[Error.s_odimages]:{ex.Message}");
                }finally{
                    conn.Close();//###
                }
            }
            return true;
        }
        
    }

    //################################################
    public class MySettings{
        public int sMSendInt; //메일 송신 주기
        public int sMSendMax; //최대 메일 송신개수.
        public String sMAccount; //메일계정.
        public String sMPwd; //메일비번.
        public int sCycle; //센서감시주기

        public MySettings(){ Clear();}
        public void Clear(){
            sMSendInt = 5000; sMSendMax = 100; sMAccount = String.Empty; sMPwd = String.Empty;
            sCycle = 1000;
        }

        public bool Read_Vsettings(){
            using ( var conn = new NpgsqlConnection(GRun.secureLoginStr))
            {
                try{
                    conn.Open();//###

                    using (var command = new NpgsqlCommand()) {
                        command.Connection = conn;
                        command.CommandText = "SELECT s_int,smail_int,smail_max,smail_addr,smail_pwd FROM vsettings ORDER BY index DESC LIMIT 1";
                        using(var reader = command.ExecuteReader())
                        {
                            while(reader.Read()){
                                sCycle = int.Parse( reader["s_int"].ToString() );
                                sMSendInt = int.Parse( reader["smail_int"].ToString() );
                                sMSendMax = int.Parse( reader["smail_max"].ToString() );
                                //sMAccount = reader["smail_addr"].ToString();
                                sMAccount = reader["smail_addr"].ToString();
                                sMPwd = reader["smail_pwd"].ToString();
                                Console.WriteLine($"[read vsettings]: {sMSendInt}, {sMSendMax}, {sMAccount}, {sMPwd}");
                            }
                        }                        
                    }                    
                }catch ( Exception ex){
                    Console.WriteLine($"[Error.vsettings]:{ex.Message}");
                }finally{
                    conn.Close();//###
                }                
            }
            return true;
        }

    }

    //################################################
   public class HostStatus{
        public int sSRun; // 영상시작여부.
        public int sVRun; // 감시시작여부.
        public int sODetect; // 물체감지여부. (비상여부)

        public HostStatus(){ Clear();}
        public void Clear(){
            sSRun = 0; sVRun = 0; sODetect = 0;
        }

        public bool Read_HostStatus(){
            using ( var conn = new NpgsqlConnection(GRun.secureLoginStr))
            {
                try{
                    conn.Open();//###

                    using (var command = new NpgsqlCommand()) {
                        command.Connection = conn;
                        command.CommandText = "SELECT srun,vrun,odetect FROM ss_status ORDER BY index DESC LIMIT 1";
                        using(var reader = command.ExecuteReader())
                        {
                            while(reader.Read()){
                                sSRun = int.Parse( reader["srun"].ToString() );
                                sVRun = int.Parse( reader["vrun"].ToString() );
                                sODetect = int.Parse( reader["odetect"].ToString() );
                                //Console.WriteLine($"[read ss_status]: srun:{sSRun}, vrun:{sVRun}, odetect:{sODetect}");
                            }
                        }                        
                    }                    
                }catch ( Exception ex){
                    Console.WriteLine($"[Error]:{ex.Message}");
                }finally{
                    conn.Close();//###
                }                
            }
            return true;
        }
    }

    public class Program
    {        

#if REAL_LATTE
        static Arduino aran0 = new Arduino();//analog pin 0        
        private static void Arduino_analogPinUpdated(int pin, int value) {
            if(0 == pin){ //analog pin 0. 온도센서                
                //int fixvalue = (int)(0.56 * (double)(value -32)); //화씨 를 섭씨로 변환.
                int fixvalue = value - 25;
                GRun.mySensor.sTem = fixvalue.ToString();
                //Console.WriteLine($"Temperature. pin[{pin}] : {fixvalue}");  //화씨로 표시됨.
            }else if( 1 == pin){ //analog pin1. 화염센서
                // 평소 0. 불꽃을 보면.. 잠시후 200이상 수치오름.
                // 거리와 불꽃의 크기에 따라 다른듯. 라이터 불의 경우 약 1m에서 2정도 값 나타남.
                GRun.mySensor.sFlar = value.ToString();
                //Console.WriteLine($"Flare. pin[{pin}] : {value}"); 
            }else if( 2 == pin){ // analog pin2. 가스센서.
                GRun.mySensor.sGas = value.ToString();
                //Console.WriteLine($"Gas. pin[{pin}] : {value}"); // 평소 100정도. 라이터가스를 뿌리면 잠시후 900이상 오른다.
            }
        }
#endif

        //############################################################
        // send mail. gmail smtp 이용하여 이미지파일첨부해 메일전송. 
        private static bool SendMail_ToUser(ref int Count)
        {
            try
            {
                // Credentials . 메일보낼 권한이 있는지. 구글계정확인.
                var credentials = new NetworkCredential(GRun.mySet.sMAccount, GRun.mySet.sMPwd);
 
                string currTime = System.DateTime.Now.ToString("yyyy/MM/dd hh:mm:ss");
                // Mail message
                var mail = new MailMessage()
                {
                    From = new MailAddress(GRun.mySet.sMAccount), //보내는사람. 자기자신.
                    //Subject = "[보안감시알림]. 이상징후 발견",
                    Subject = String.Format("[보안감시알림]. 이상징후 발견.{0}",Count),
                    Body = String.Format("보안감시 알림이 등록되었습니다.\r\n감지시간:{0}\r\n온도:{1}, 가스:{2}, 화염:{3}\r\n",
                    currTime, GRun.mySensor.sTem, GRun.mySensor.sGas, GRun.mySensor.sFlar)
                };
 
                 //첨부파일. 이미지 붙이기. 
                 System.Net.Mail.Attachment attachment; //첨부파일을 위함.
                attachment = new System.Net.Mail.Attachment(GRun.myDImg.sPath+"\\"+ GRun.myDImg.sFName); //
                mail.Attachments.Add(attachment);//메일에 첨부파일 붙이기.
                    
                mail.To.Add(new MailAddress(GRun.mySet.sMAccount));//받는사람주소=>자기자신.
 
                // Smtp client
                var client = new SmtpClient()
                {
                    Port = 587,
                    DeliveryMethod = SmtpDeliveryMethod.Network,
                    UseDefaultCredentials = false,
                    Host = "smtp.gmail.com",
                    EnableSsl = true,
                    Credentials = credentials
                };
 
                // Send it...         
                client.Send(mail);
            }
            catch (Exception ex)
            {
                Console.WriteLine("[Error] in sending email: " + ex.Message);
                //Console.ReadKey();
                return false;
            }
 
            Console.WriteLine("[success] Email sccessfully sent.");
            //Console.ReadKey();
            return true;
        }

        static void Main(string[] args)
        {
#if false
            //pin0에서 1번 아날로그 데이터를 읽는다. 
            int Value = aran0.analogRead(0);//Read the state of pin 0
            Console.WriteLine(Value);

            //analog 핀들에서 데이터를 읽기위해 이벤트 처리기 등록.
            aran0.analogPinUpdated += Arduino_analogPinUpdated;

            aran0.pinMode(9, Arduino.OUTPUT);//Set the digital pin 9 as output
            aran0.pinMode(10, Arduino.OUTPUT);//Set the digital pin 10 as output

            while (true)
            {
                // ==== set the led on or off
                aran0.digitalWrite(9, Arduino.HIGH);//set the LED　on
                Thread.Sleep(500);//delay a seconds
                aran0.digitalWrite(10, Arduino.HIGH);//set the LED　on
                Thread.Sleep(500);//delay a seconds
                aran0.digitalWrite(9, Arduino.LOW);//set the LED　off
                Thread.Sleep(500);//delay a seconds
                aran0.digitalWrite(10, Arduino.LOW);//set the LED　off
                Thread.Sleep(500);//delay a seconds
            }
#endif
            //주)) 이프로그램은 host에서 Run->보안감시 시작 을 누를 경우 실행되도록 할 예정임. 
            // 이 프로그램의 종료는 보안감시 중지를 누르거나, 또는 Run을 Stop한 경우에 이루어 질 것임.
            int myCycle = 1000;
            int nSendMailCount = 0;
            int nMailCheckLoop = 0;
            int FirstLatteValue = 0;

            Process[] procs = Process.GetProcessesByName("LatteTest"); //프로그램 중복실행 방지.
            if(procs.Length > 1){ return;} //이미 프로그램이 실행되고 있는 중.       

            GRun.InitGRun();
            GRun.mySet.Read_Vsettings(); //설정값. 

            myCycle = GRun.mySet.sCycle; //센서 감시 주기.
            if( 7 > GRun.mySet.sMSendInt ){GRun.mySet.sMSendInt = 7;}
            if( 200 < GRun.mySet.sMSendMax){ GRun.mySet.sMSendMax = 200;}
            else if( 1 > GRun.mySet.sMSendMax){ GRun.mySet.sMSendMax = 1;}
            //SendMail_ToUser(ref GRun.mySet.sMAccount, ref GRun.mySet.sMPwd,ref myCycle); //send mail. for test.
            
#if REAL_LATTE
            //pin0에서 1번 아날로그 데이터를 읽는다. 
            FirstLatteValue = aran0.analogRead(0);//Read the state of pin 0
            //Console.WriteLine(Value);

            //analog 핀들에서 데이터를 읽기위해 이벤트 처리기 등록.
            aran0.analogPinUpdated += Arduino_analogPinUpdated;

            aran0.pinMode(9, Arduino.OUTPUT);//Set the digital pin 9 as output
            aran0.pinMode(10, Arduino.OUTPUT);//Set the digital pin 10 as output

            aran0.digitalWrite(9, Arduino.LOW);//set the Red LED　off
            Thread.Sleep(100);//delay a seconds
            aran0.digitalWrite(10, Arduino.LOW);//set the Blue LED　off
#endif          

            while(true){
                Thread.Sleep(myCycle);//일단 센서감시 주기만큼 지연.
                GRun.hostStatus.Read_HostStatus();//host 프로그램(C++.mfc)의 실행상태.감시상태.
                if(0 == GRun.hostStatus.sSRun){break; } // 이 프로그램이 종료될것임.
                else{ //영상 재생중..
                    if( 0 == GRun.hostStatus.sVRun){ 
                        //Thread.Sleep(myCycle);  continue;
                        break; // 감시시작이 아닐 경우. 이 프로그램이 종료될 것임. 
                    }else{//아래는 감시시작을 누른 경우에만 실행됨.
#if REAL_LATTE
                        //### 센서값들을 DB에 저장한다. 
                        GRun.mySensor.Insert_SensorValue(); //
#endif                        
                        if( 0 == GRun.hostStatus.sODetect){
#if REAL_LATTE
                            aran0.digitalWrite(10, Arduino.HIGH);//set the Blue LED　on
                            aran0.digitalWrite(9, Arduino.LOW);//set the Red LED　off
#endif
                            nMailCheckLoop = 0;

                        }else{ //물체(침입)감지. 또는 센서값 감시범위 초과.
#if REAL_LATTE                            
                            aran0.digitalWrite(9, Arduino.HIGH);//set the Red LED　on
                            aran0.digitalWrite(10, Arduino.LOW);//set the Blue LED　off
#endif
                            // N초에 한번씩 최대 N회 메일을 전송해 준다.
                            if( 0 == nMailCheckLoop%GRun.mySet.sMSendInt && nSendMailCount < GRun.mySet.sMSendMax){
                                nSendMailCount++;

                                //### host가 저장한 가장최근의 이미지 경로를 읽어온다. 
                                GRun.myDImg.Read_DImagePath();
                                SendMail_ToUser(ref nSendMailCount); //send mail.                                
                            }
                            nMailCheckLoop++;
                        }
                    }//else. 감시중인 상태임.
                }//else. host에서 영상 run 상태임.
            }//end of while.
#if REAL_LATTE
            aran0.digitalWrite(9, Arduino.LOW);//set the Red LED　off
            Thread.Sleep(100);//delay a seconds
            aran0.digitalWrite(10, Arduino.LOW);//set the Blue LED　off
#endif

        }//main
    }

}
