#ifndef _PROFILER_H_
#define _PROFILER_H_

// include the base class definition
#include "EventProcessor.h"
#include "EventProcessorImp.h"
#include "Message.h"
#include "ProfMSG.h"
#include "PerfProfMSG.h"
#include <map>
#include <iostream>
#include <iomanip>

class ProfilerImp : public EventProcessorImp {
    typedef int64_t IntType;
    typedef std::map<std::string, IntType> CounterMap;
    typedef std::map<std::string, CounterMap> GroupMap;
    GroupMap cMap; // the map of counters

    // Map of counters whose value is the most recent value seen
    GroupMap progMap;

    bool suppressOutput;

    EventProcessor logger;

    static constexpr int groupColWidth = 20;

    int64_t lastCpu;
    int64_t lastTick;

    double beginTime;

    void AddCounter(PCounter& cnt);
    void AddCounterProg(PCounter& cnt);
    void PrintCounters(const int64_t newCpu, const int64_t newClock);

    void HumanizeNumber( IntType value, std::string& outVal );

    protected:

    virtual void PreStart(void) override;

    public:

    ProfilerImp( bool suppress = false );
    ~ProfilerImp();

    void SuppressOutput(bool v);

    void profileMessage_H(ProfileMessage &msg);
    void profileSetMessage_H(ProfileSetMessage &msg);
    void profileInstantMessage_H(ProfileInstantMessage &msg);
    void ProfileProgressMessage_H(ProfileProgressMessage &msg);
    void profileProgressSetMessage_H(ProfileProgressSetMessage &msg);
    void profileIntervalMessage_H(ProfileIntervalMessage &msg);
    void perfTopMessage_H(PerfTopMessage &msg);


    ACTOR_HANDLE
        HANDLER(ProfileMessage, profileMessage_H, 2)
        HANDLER(ProfileSetMessage, profileSetMessage_H, 2)
        HANDLER(ProfileIntervalMessage, profileIntervalMessage_H, 2)
        HANDLER(ProfileInstantMessage, profileInstantMessage_H, 2)
        HANDLER(ProfileProgressMessage, profileProgressMessage_H, 2)
        HANDLER(ProfileProgressSetMessage, profileProgressSetMessage_H, 2)
        HANDLER(PerfTopMessage, perfTopMessage_H, 2)
    END_HANDLE
};


class Profiler : public EventProcessor {

    public:
        Profiler( bool suppress = false ){
            evProc = new ProfilerImp(suppress);
        }

        void SuppressOutput(bool v) {
            ProfilerImp * ep = dynamic_cast<ProfilerImp *>(evProc);
            FATALIF(ep == nullptr, "Attempted to call SuppressOutput on invalid Profiler");
            ep->SuppressOutput(v);
        }

        virtual ~Profiler(){}

};

// Global variables
extern Profiler globalProfiler;

// Inline methods
inline void ProfilerImp::AddCounter(PCounter& cnt){
    IntType value = cnt.get_value();
    const std::string& counter = cnt.get_name();
    const std::string& group = cnt.get_group();
    cMap[group][counter] += value;
}

inline void ProfilerImp::AddCounterProg(PCounter& cnt) {
    IntType value = cnt.get_value();
    const std::string& counter = cnt.get_name();
    const std::string& group = cnt.get_group();
    progMap[group][counter] = value;
}

inline
void ProfilerImp :: HumanizeNumber( IntType value, std::string& outVal ) {
    const char * suffList = " KMGTPEZY";
    int index = 0;

    // Shift it down if the value is in danger of using more than 4 digits.
    while( value > 9999 ) {
        value >>= 10;
        ++index;
    }

    std::ostringstream str;
    str << std::setw(5) << value << suffList[index];
    outVal = str.str();
}


#endif // _PROFILER_H_
