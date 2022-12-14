/**
  ******************************************************************************
  * File Name          : main.cpp
  * Description        : 
  ******************************************************************************
  * @attention
  *
  * Copyright (C) Sarun Kumar M V [https://github.com/mvsarun6] [mvsarun6@gmail.com].  All Rights Reserved.
  *
  * Liscence info
  *
  ******************************************************************************
  */
#include <iostream>
#include <iomanip>
#include <vector>

typedef unsigned char uint8;
typedef unsigned int uint32;
typedef int int32;
typedef short int16;
typedef unsigned short uint16;

class BER_TLV_Parser
{
private:
  int parser(std::vector<uint8> data,int32 primitive);
  std::vector<uint8> prvdata;

public:
    BER_TLV_Parser(){
      std::cout<<"\nError : Invalid input, call parser with std::vector input\n";
    }

    BER_TLV_Parser(std::vector<uint8> in): prvdata { in }{
      std::cout<<"\nInput Data Size = "<<prvdata.size();
    }

    ~BER_TLV_Parser(){
    }

    void display_data()
    {
        std::cout<<"\nFull data input: ";
      for(auto &ref : prvdata)
      {
        std::cout<<std::setfill('0') << std::setw(2)<<std::hex<<(int)ref<<" ";
      }
        std::cout<<"\n\n";
    }

    int lets_parse();

};

#define TAG_TYPE_BIT 0x60  //bit 6
#define BITSCHECK_FOR_TAGBYTE2 0x1F //bit 1-5 : 0001 1111 
#define TAGBYTE2_CHECKBIT8 0x80 //bit 8
#define TAGBYTE2_CHECKBITTAG 0x7F //bit 1-7
#define NOOF_LBYTES 2

int BER_TLV_Parser::lets_parse()
{
  parser(prvdata, -1);
  std::cout<<"\n\nparsing done\n";
}

int BER_TLV_Parser::parser(std::vector<uint8> data, int32 primitive)
{
      int T_end=0;
      int L_start=0;
      int L_end=0;
      int =V_start0;
      int V_end=-1;
      int isprimitive = 0;
      size_t packet_length = 0;

      std::vector<uint8>::iterator iter = data.begin();

      if(data.size()<2 || data.at(0)==0x00 || data.at(0)==0xFF)
      {
          std::cout<<"\nError : Invalid packet header\n";
          return 0;
      }
    
      /************* TAG *************/
      {
          //skip this if incoming is not BER (SIMPLE TLV)
          if((primitive!=1)  &&   (data.at(0)&TAG_TYPE_BIT)==TAG_TYPE_BIT)
          {
            //constructed
            isprimitive = 0;
            std::cout<<"\n[constructed]: ";
          }
          else
          {
            //primitive
            isprimitive = 1;
            std::cout<<"\n[primitive]  : ";
          }

          iter = data.begin();
          if((data.at(0) & BITSCHECK_FOR_TAGBYTE2)==BITSCHECK_FOR_TAGBYTE2)
          {
            //bit 1-5 set to 1 : 2 or more bytes tag
            std::cout<<" TAG= "<<std::setfill('0') << std::setw(2)<< (int)data.at(0);
            iter++;
                for( ;iter!=data.end();iter++)
                {                  
			            //TBD : Compute Tag here TAGBYTE2_CHECKBITTAG of each byte
                  std::cout<<std::setfill('0') << std::setw(2)<< (int)*iter;
                  T_end++;
                  if( (*iter&TAGBYTE2_CHECKBIT8)==1)
                  {
                  }
                  else
                  {
                    //Tag ends
                    break;			    
                  }
                }	      
          }
          else
          {
              std::cout<<" TAG= "<<std::setfill('0') << std::setw(2)<<(int)data.at(0);
           //   std::cout<<"\ntag = "<<(data.at(0) & BITSCHECK_FOR_TAGBYTE2);
              iter++;
          }
          L_start=T_end+1;
      }

      /************* LENGTH *************/
      {
          if(data.at(L_start)&0x80)
          {
            int no_lengthbytes = (data.at(L_start)&0x7F);
            packet_length=0;
            L_end = L_start+no_lengthbytes;
            for(int i=L_start; i<L_end;i++)
            {
              packet_length += static_cast<size_t>(data.at(i));
            }
            if(packet_length<128)
            {
              std::cout<<"\nError : Invalid length data\n";
            }
          }
          else
          {
            iter++;
            L_end = L_start;
            packet_length = data.at(L_start);//(uint16) (((data.at(L_start)&0x7F)<<8)|at(L_end))
          }
          std::cout<<" length= "<<std::setfill('0') << std::setw(2)<<packet_length;
          //validate length
          V_start = L_end+1;
          if(packet_length==0 || data.size()<=(L_end+1))
          {
              std::cout<<"\nError : Invalid length data\n"<<L_end;
               return 0;
          }
          else if(packet_length==0)
          {
               return L_end+1;
          }
      }
      V_end = V_start+packet_length-1;

      if(V_end>data.size())
      {
              std::cout<<"\nError : Invalid length something wrong\n"<<L_end;
               return 0;
      }
      /************* VALUE *************/
      int tot_bytes_processed=0;
      {
        if(primitive==1 || ((primitive !=-1) && (isprimitive==1)))
        {
          std::cout<<" Data= ";
          for(auto it = data.begin()+V_start; it <= (data.begin()+V_end); it++)
          {
              std::cout<<std::setfill('0') << std::setw(2)<<(int)*it<<" ";
          }
          //xx std::cout<<"\nreturning  1 = "<<V_end+1;
          return V_end+1;
        }

        int len_processed=0;
        while(((int)len_processed)<((int)packet_length))
        {
            int ret=0;
            std::vector<uint8> dummyvec{data};
            //xx   std::cout<<"\n V_start+len_processed="<<(V_start+len_processed)<<" Vec size ="<<data.size();
            dummyvec.erase(dummyvec.begin(),dummyvec.begin()+V_start+len_processed);
            ret = parser(dummyvec,isprimitive);  //Recursive Call          
           //xx  std::cout<<"\n new ret value ="<<ret<<"  prev processed ="<<len_processed;
            if(ret==0)
            {
              break;
            }
            len_processed += ret;
           //xx std::cout<<"\nlen_processed ="<<len_processed<<"  of packet_length ="<<packet_length<< " of Vec size ="<<data.size();;
           //xx std::cout<<"\n Remaining bytes to process : "<<packet_length-len_processed;
            if(len_processed>packet_length)
            {
             //xx std::cout<<"\nError : Length info\n"<<L_end;
              break;
            }
        }
        tot_bytes_processed = V_start+len_processed;
      }
    //xx  std::cout<<"\nreturning  2 = "<<tot_bytes_processed;
      return tot_bytes_processed;
}

int main()
{

  std::vector<uint8> indata = <put data here>;

  BER_TLV_Parser parser(indata);
  parser.display_data();

  parser.lets_parse();


  int v;
	std::cout<<"\nPGM END"<<std::endl;
  std::cin>>v;
	return 0;
}

