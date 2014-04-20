
#ifndef __QTRACEFORMATTER__
#define __QTRACEFORMATTER__

#include <string>

#include "QCallTrace.h"

class QTraceFormatter
{
public:

    void FormatTrace(const QCallTrace &trace, std::string &strTrace) const
    {
        strTrace = "-->";

        if(!trace.m_className.empty())
        {
            strTrace.append(trace.m_className);
            strTrace.append("::");
        }

        strTrace.append(trace.m_functionSignature);
        strTrace.append("\n");

        if(trace.m_pParameters)
        {
            for(int i = 0; i < trace.m_numParameters; ++i)
            {
                strTrace.append("     -{");
                {
                    std::ostringstream st;
                    st << i;
                    strTrace.append(st.str());
                }
                strTrace.append("}");
                strTrace.append(trace.m_pParameters[i].m_pName);
                strTrace.append("=");
                strTrace.append(trace.m_pParameters[i].m_strValue);
                strTrace.append("\n");
            }
        }
    }

    void FormatTraceHeader(const QCallTrace &trace, std::string &strTrace) const
    {
        strTrace = "";
    }

    void FormatTraceFooter(const QCallTrace &trace, std::string &strTrace) const
    {
        strTrace = "";
    }

    void FormatCallStackTraceHeader(const QCallStackTrace &trace, std::string &strTrace) const
    {
        strTrace = "Call stack trace for Thread: ";

        std::ostringstream st;
        st << trace.ThreadId;
        std::string strThreadId = st.str();

        strTrace.append(strThreadId).append("\n");
    }

    void FormatCallStackTraceFooter(const QCallStackTrace &trace, std::string &strTrace) const
    {
        strTrace = "\nEnd of stack trace information.";
    }
};


#endif // __QTRACEFORMATTER__