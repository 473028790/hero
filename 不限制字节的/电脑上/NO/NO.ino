#include <ESP8266WiFi.h>
#include <espnow.h>

uint8_t esp_HAVE_address[] = {0xAC, 0x0B, 0xFB, 0xE5, 0xD3, 0x97};//esp01s的mac地址（有高温胶布）
uint8_t esp_NO_address[] = {0xB4, 0x8A, 0x0A, 0xE9, 0xAD, 0x81};//esp01s的mac地址（没有高温胶布）

//接收信息时的回调函数，每次接收信息会自动调用该函数
uint8_t data_to[2]={0x80,0x7F};
uint8_t data_rc[50];
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) 
{
    memcpy(&data_rc, incomingData, sizeof(data_rc));
     //Serial.write(data_to[0]);
     for(int i=0;i<len;i++)
     {
            Serial.write(data_rc[i]);
      }
     //Serial.write(data_to[1]);
}

void setup() {
  Serial.begin(115200);//初始化串口，设置其波特率为115200
  WiFi.mode(WIFI_STA);//设置WIFI模式为STA
  WiFi.disconnect();//断开WIFI连接
  
  // Init ESP-NOW
  esp_now_init();
  
  //双向通信时需要设定角色为 ESP_NOW_ROLE_COMBO
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  
  //与模块配对
  esp_now_add_peer(esp_HAVE_address, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  //执行完这个函数，每次接收数据就会自动调用回调函数了
  esp_now_register_recv_cb(OnDataRecv);
}
uint8_t data_to_send ;
uint8_t data[100];
uint8_t data_now[100];
int i=0;
int data_long=0;
void loop() 
{
    if (Serial.available() > 0) {
        data[i]= Serial.read();
        if(data[0]!=0x81)
        {
            i=0;
         }
         else
         {
             i++;
             if(i>1 && data[i-1]==0X7E)
             {
                data_long=i-2;
                //Serial.write(data[1]);
                //Serial.write(data[2]);
                
                for(int a=1;a<data_long+1;a++)
                {
                  data_now[a-1]=data[a];
                  if(a==data_long)
                  {
                      i=0;
                   }
                }
                  esp_now_send(esp_HAVE_address, data_now, sizeof(data_now));
              }
          }
    }
  }



  

