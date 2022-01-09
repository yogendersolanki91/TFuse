using Serilog;
using Serilog.Configuration;
using Serilog.Core;
using Serilog.Events;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Text;

namespace TFuse
{


    public class InvocationContextEnricher : ILogEventEnricher
    {
        public InvocationContextEnricher(string sourceContext, string callerMemberName, string callerFilePath, int callerLineNumber)
        {
            SourceContext = sourceContext;
            CallerMemberName = callerMemberName;
            CallerFilePath = callerFilePath;
            CallerLineNumber = callerLineNumber;
        }

        public string SourceContext { get; protected set; }
        public string CallerMemberName { get; protected set; }
        public string CallerFilePath { get; protected set; }
        public int CallerLineNumber { get; protected set; }

        public static string SourceContextPropertyName { get; } = Constants.SourceContextPropertyName;
        public static string CallerMemberNamePropertyName { get; } = "CallerMemberName";

        public static string CallerFilePathPropertyName { get; } = "CallerFilePath";
        public static string CallerLineNumberPropertyName { get; } = "CallerLineNumber";

        public void Enrich(LogEvent logEvent, ILogEventPropertyFactory propertyFactory)
        {
            logEvent.AddPropertyIfAbsent(new LogEventProperty(SourceContextPropertyName, new ScalarValue(SourceContext)));
            logEvent.AddPropertyIfAbsent(new LogEventProperty(CallerMemberNamePropertyName, new ScalarValue(CallerMemberName)));
            logEvent.AddPropertyIfAbsent(new LogEventProperty(CallerFilePathPropertyName, new ScalarValue(CallerFilePath)));
            logEvent.AddPropertyIfAbsent(new LogEventProperty(CallerLineNumberPropertyName, new ScalarValue(CallerLineNumber)));
        }
    
        public class CallerEnricher : ILogEventEnricher
        {
            public void Enrich(LogEvent logEvent, ILogEventPropertyFactory propertyFactory)
            {
                var skip = 3;
                while (true)
                {
                    var stack = new StackFrame(skip);
                    if (!stack.HasMethod())
                    {
                        logEvent.AddPropertyIfAbsent(new LogEventProperty("Caller", new ScalarValue("<unknown method>")));
                        return;
                    }

                    var method = stack.GetMethod();
                    if (method.DeclaringType.Assembly != typeof(Log).Assembly)
                    {
                        var caller = $"{method.DeclaringType.FullName}.{method.Name}";
                        logEvent.AddPropertyIfAbsent(new LogEventProperty("Caller", new ScalarValue(caller)));
                    }

                    skip++;
                }
            }
        }

       
    }    
}
