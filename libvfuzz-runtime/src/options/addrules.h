#pragma once

#include <map>
#include <vector>
#include <optional>
#include <memory>
#include <base/exception.h>
#include <base/types.h>

namespace vfuzz {
namespace options {

class SensorData {
};

class Constraint {
    private:
        const bool invert;
    protected:
        virtual bool test(const SensorData& sensorData) const = 0;
    public:
        Constraint(const bool invert);
        virtual ~Constraint() = default;
        bool Test(const SensorData& sensorData) const;
};

class ConstraintLimit {
    public:
        enum Operator {
            OPERATOR_EQ,
            OPERATOR_LT,
        };
};

class SensorConstraint : public Constraint {
    private:
        SensorID sid;
        std::optional<ConstraintLimit> limit;
    protected:
        bool test(const SensorData& sensorData) const override;
    public:
        SensorConstraint(const bool invert, const SensorID sid, std::optional<ConstraintLimit> limit = {});
};

class GeneratorConstraint : public Constraint {
    private:
        GeneratorID gid;
    protected:
        bool test(const SensorData& sensorData) const override;
    public:
        GeneratorConstraint(const bool invert, const GeneratorID gid);
};

class ConstraintSet {
    private:
        std::vector<std::shared_ptr<Constraint>> constraints;
    public:
        ConstraintSet(void) = default;
        ~ConstraintSet() = default;
        void Add(const std::shared_ptr<Constraint> constraint);
        bool Test(SensorData& sensorData) const;
};

class Selector {
    private:
        const std::vector<GeneratorID> gids;
        void validate(void) const;
    public:
        Selector(const std::vector<GeneratorID> gids);
        bool Select(const GeneratorID gid) const;
};

class RewriteRule {
    private:
        const std::map<GeneratorID, GeneratorID> mapping;
        void validate(void) const;
    public:
        RewriteRule(const std::map<GeneratorID, GeneratorID> mapping);
        GeneratorID MapTo(const GeneratorID gid) const;
};

class AddRuleConstructionException : public Exception {
    public:
        AddRuleConstructionException(const std::string msg) : Exception(msg) { }
};

class AddRule {
    private:
        std::optional<std::shared_ptr<ConstraintSet>> constraintSet = {};
        std::optional<std::shared_ptr<Selector>> selector = {};
        std::optional<std::shared_ptr<RewriteRule>> rewriteRule = {};

        void constraintSetFromString(std::string constraintStr);
        void selectorFromString(const std::string selectorRuleStr);
        void rewriteRuleFromString(const std::string rewriteRuleStr);
    public:
        AddRule(std::string addRuleStr);
        ~AddRule() = default;
        GeneratorID MapTo(const GeneratorID gid) const;
        bool Select(const GeneratorID gid) const;
};

} /* namespace options */
} /* namespace vfuzz */
