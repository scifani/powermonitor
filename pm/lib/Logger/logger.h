#ifndef __LOGGER__
#define __LOGGER__

#include <Arduino.h>
#include <DateTime.h>

#include <sstream>
#include <vector>


class ILogWriter {
public:
    virtual void write(const char* message) = 0;
};

class SerialWriter : public ILogWriter {
public:
    SerialWriter(HardwareSerial* serial) {
        _serial = serial;
    }

    void write(const char* message) {
        _serial->write(message);
    }

private:
    HardwareSerial* _serial;
};


class Logger {

    public:

        enum Severity {
            ERROR,
            WARNING,
            INFO,
            DEBUG
        };

        void add_writer(ILogWriter* writer) {
            _writers.push_back(writer);
        }

        void error(const char* format, ...) {

            char buffer[1024];
            va_list args;
            va_start(args, format);
            vsnprintf(buffer, sizeof(buffer), format, args);
            va_end(args);

            trace(Severity::ERROR, buffer);
        }

        void warning(const char* format, ...) {

            char buffer[1024];
            va_list args;
            va_start(args, format);
            vsnprintf(buffer, sizeof(buffer), format, args);
            va_end(args);

            trace(Severity::WARNING, buffer);
        }

        void info(const char* format, ...) {

            char buffer[1024];
            va_list args;
            va_start(args, format);
            vsnprintf(buffer, sizeof(buffer), format, args);
            va_end(args);

            trace(Severity::INFO, buffer);
        }

        void debug(const char* format, ...) {

            char buffer[1024];
            va_list args;
            va_start(args, format);
            vsnprintf(buffer, sizeof(buffer), format, args);
            va_end(args);

            trace(Severity::DEBUG, buffer);
        }

    private:

        void trace(Severity severity, const char* message) {

            DateTimeClass dt;
            String timeString = dt.format("%FT%TZ");

            std::stringstream ss;
            ss << dt.format("%FT%TZ").c_str();
            ss << " [" << SeverityStr(severity) << "] - ";
            ss << message << "\n";

            for(size_t i=0; i < _writers.size(); i++) {
               _writers[i]->write(ss.str().c_str());
            }
        }

        static const char* SeverityStr(Severity severity) {
            switch (severity)
            {
            case ERROR:
                return "ERROR";
            case WARNING:
                return "WARNING";
            case INFO:
                return "INFO";
            case DEBUG:
                return "DEBUG";
            default:
                return "";
            }
        }

        std::vector<ILogWriter*> _writers;
};

#endif // __LOGGER__