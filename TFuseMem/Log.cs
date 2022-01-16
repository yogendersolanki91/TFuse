/*
 ***************************************************************************** 
 * Author: Yogender Solanki <yogendersolanki91@gmail.com> 
 *
 * Copyright (c) 2011 Yogender Solanki
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************
 */
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Serilog;
using Serilog.Configuration;
using Serilog.Core;
using Serilog.Events;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Text;
using Thrift.Processor;
using static TFuse.InvocationContextEnricher;

namespace TFuse
{

    static class LoggerCallerEnrichmentConfiguration
    {
        public static LoggerConfiguration WithCaller(this LoggerEnrichmentConfiguration enrichmentConfiguration)
        {
            return enrichmentConfiguration.With<CallerEnricher>();
        }
    }

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
                        var caller = $"{method.DeclaringType.Name}.{method.Name}";
                        logEvent.AddPropertyIfAbsent(new LogEventProperty("Caller", new ScalarValue(caller)));
                    }

                    skip++;
                }
            }
        }


    }
    class TFuseLog
    {
        private readonly static string logOutPut = "{Timestamp:yyyy-MM-ddTHH:mm:ss.fff} {Level:u4} [Sub={Caller}] {Message}{NewLine}{Exception}";

        public static void InitLoggers()
        {
            Log.Logger = new LoggerConfiguration().MinimumLevel.Debug()
               .Enrich.WithCaller()
               .WriteTo.Console(theme: Serilog.Sinks.SystemConsole.Themes.SystemConsoleTheme.Literate, outputTemplate: logOutPut)
               .CreateLogger();
        }
        public static IHost BuildHost()
        {
            return new HostBuilder()
            .ConfigureServices(services => { services.AddSingleton<FuseService.IAsync, TFuseMem>(); services.AddSingleton<ITAsyncProcessor, FuseService.AsyncProcessor>(); })
            .UseSerilog()
            .Build();
        }
    }


}    

