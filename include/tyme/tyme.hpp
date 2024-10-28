//////////////////////////////////////////////////////////////////////////////
//     
//          filename            :   tyme.h
//          License             :   GNU 
//          Author              :   Lio
//          Change History      :
//          Processor           :   ARM
//          Hardware            :		
//          Complier            :   ARM
//          Company             :
//          Dependencies        :
//          Description         :
//          brief               :	
//
//////////////////////////////////////////////////////////////////////////////

#pragma once
#include <iostream>
#include <cstring>
#include <string_view>
#include <chrono>
#include <cstdint>

namespace TYME{

    struct Time_t
    {
            explicit Time_t();
            ~Time_t();
            void delay_ms(const int64_t);
            const std::string get_tyme();
            bool getHourNTP(const std::string_view& , std::chrono::system_clock::time_point& ) ;
            int timeUpdate() ;
        //private:
            //int64_t m_delay{0};
    };


    void    delay          ( int64_t );
    void    delay_ms       ( int64_t );
    void    delay_us       ( int64_t );
    void    delay_sec      ( int64_t );


}//end time