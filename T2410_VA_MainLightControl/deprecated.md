//------------------------------------------------------------------------------------------------------
Excluded  from main;
//eep_initialize(EEP_SIZE);
// edog_initialize(EDOG_I2C_ADDR);   the circuit will require a pulldown  for the edog to function :(
// helper_initialize_data();
// edog_test_eeprom_write_read();
helper_initialize_data();
  

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------


// void autom_set_time(uint8_t hour, uint8_t minute)
// {
//     autom_cntrl.hour = hour;
//     autom_cntrl.minute = minute;
// }

// uint8_t autom_get_hour(void)
// {
//     return autom_cntrl.hour;
// }

// uint8_t autom_get_minute(void)
// {
//     return autom_cntrl.minute;
// }

#define TIME_NBR_OF_FIELDS 6

// bool autom_parse_time(String *tstrp)
// {
//     bool do_continue = true;
//     uint8_t pos[TIME_NBR_OF_FIELDS + 1];
//     String field[TIME_NBR_OF_FIELDS];
//     // String year_str, month_str, day_str, hour_str, minute_str;

//     for (uint8_t i = 0; i < TIME_NBR_OF_FIELDS+1; i++) pos[i] = 0;

//     Serial.println(*tstrp);
//     tstrp->trim();
//     //Serial.println(tstrp->length());

//     if(!tstrp->startsWith("<") || !tstrp->endsWith(">")) do_continue = false;

//     if (do_continue)
//     {
//       if (tstrp->indexOf("C1T=") != 1) do_continue = false;
//     }
//     // <C1T=:2024/5/7;9:28:16>
//     if (do_continue)
//     {
//       pos[0] = tstrp->indexOf(':');
//       pos[1] = tstrp->indexOf('/',pos[0]+1);
//       pos[2] = tstrp->indexOf('/',pos[1]+1);
//       pos[3] = tstrp->indexOf(';',pos[2]+1);
//       pos[4] = tstrp->indexOf(':',pos[3]+1);
//       pos[5] = tstrp->indexOf(':',pos[4]+1);
//       pos[6] = tstrp->indexOf('>',pos[5]+1);

//       Serial.print("Date-Time pos: ");
//       for (uint8_t i = 0; i < TIME_NBR_OF_FIELDS+1 ; i++)
//       {
//           if ((pos[i] == 0) || (pos[i] == 255)) do_continue = false;  
//           Serial.printf("%d=%d ",i, pos[i]);
//       }
//       Serial.println();
//     }
//     //Serial.printf("ix= %d %d %d\n", i1, i2, i3);
//     if (do_continue)
//     {
//         for (uint8_t i = 0; i < TIME_NBR_OF_FIELDS; i++)
//         {
//             field[i] = tstrp->substring(pos[i]+1,pos[i+1]);
//         }
//     }

//     if (do_continue)
//     {
//       main_ctrl.time.year   = field[0].toInt();
//       main_ctrl.time.month  = field[1].toInt();
//       main_ctrl.time.day    = field[2].toInt();
//       main_ctrl.time.hour   = field[3].toInt();
//       main_ctrl.time.minute = field[4].toInt();
//       main_ctrl.time.second = field[5].toInt();

//       Serial.printf("Date-Time accepted: %04d-%02d-%02d %02d:%02d:%02d\n\r",
//           main_ctrl.time.year,
//           main_ctrl.time.month,
//           main_ctrl.time.day,
//           main_ctrl.time.hour,
//           main_ctrl.time.minute,
//           main_ctrl.time.second
//       );
//     }
//     // Serial.println(do_continue);
//     return do_continue;
// }


        // case 20:
        //     autom_task_handle. = 20;
        //     if (SerialClock.available())
        //     {
        //         str = SerialClock.readStringUntil('\n');
        //         if (str.length()> 0)
        //         {
        //             if (autom_parse_time(&str))
        //             {
        //               // autom_cntrl.th->wd_cntr = 0;
        //               if ((main_ctrl.time.hour != autom_cntrl.last_time.hour) ||
        //                   (main_ctrl.time.minute > autom_cntrl.last_time.minute ))
        //               {
        //                 autom_cntrl.last_time.hour = main_ctrl.time.hour;
        //                 autom_cntrl.last_time.minute = main_ctrl.time.minute;
        //                 autom_task_handle. = 50;
        //               }
        //             }
        //         }
        //         supervisor_clr_cntr(SUPER_ERR_GET_TIME);
        //     }
        //     else
        //     {
        //         if (autom_cntrl.iter_cntr > autom_cntrl.next_get_time)  
        //         {
        //             Serial.println("No get time response -> WD Reset");
        //             //autom_cntrl.th->wd_cntr = autom_cntrl.th->wd_limit; 
        //             autom_task_handle. = 200;  
        //         }
        //     }
        //     break;


