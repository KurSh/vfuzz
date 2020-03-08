#pragma once

#include <base/types.h>
#include <logger/loggerchain.h>
#include <optional>
#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include <functional>

namespace vfuzz {
namespace sensor {

class BaseProcessor
{
    private:
        bool disabled;
        std::optional<std::shared_ptr<LoggerChain>> loggerChain;
    protected:
        std::vector<Value> vals;
        virtual void start(void);
        virtual void stop(void);
        virtual void receiveInput(const SensorID ID, Value data) = 0;
        void onNew(const SensorID ID, const Value prev, const Value cur) const;
        void defaultOnAddVal(const Value data);
        std::function<void(const Value data)> onAddVal;
        std::string name;
    public:
        BaseProcessor(std::optional<std::shared_ptr<LoggerChain>> loggerChain = std::nullopt);
        virtual ~BaseProcessor() = default;
        std::vector<Value> GetValues(void) const;
        size_t GetNumValues(void) const;
        template <class Callback> void GetValues(Callback CB) const
        {
            for (const auto& val : vals ) {
                CB(val);
            }
        }
        void Start(void);
        void Stop(void);
        void ReceiveInput(const SensorID ID, const Value data);
        void SetOnAddVal(const std::function<void(const Value data)> _onAddVal);
        void SetName(const std::string& name);
};

} /* namespace sensor */
} /* namespace vfuzz */
