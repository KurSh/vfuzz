#include <options/addrules.h>
#include <util/string/string.h>
#include <boost/lexical_cast.hpp>
#include <algorithm>

namespace vfuzz {
namespace options {

Constraint::Constraint(const bool invert) :
    invert(invert) {
}

bool Constraint::Test(const SensorData& sensorData) const {
    const bool ret = test(sensorData);
    return invert == false ? ret : !ret;
}

SensorConstraint::SensorConstraint(const bool invert, const SensorID sid, std::optional<ConstraintLimit> limit) :
    Constraint(invert), sid(sid), limit(limit) {
}

bool SensorConstraint::test(const SensorData& sensorData) const {
    /* TODO */
    return true;
}

GeneratorConstraint::GeneratorConstraint(const bool invert, const GeneratorID gid) :
    Constraint(invert), gid(gid) {
}

bool GeneratorConstraint::test(const SensorData& sensorData) const {
    /* TODO */
    return true;
}

void ConstraintSet::Add(const std::shared_ptr<Constraint> constraint) {
    constraints.push_back(constraint);
}

bool ConstraintSet::Test(SensorData& sensorData) const {
    /* TODO */
    return true;
}

Selector::Selector(const std::vector<GeneratorID> gids) :
    gids(gids) {

    validate();
}

void Selector::validate(void) const {
    {
        /* Create copy of gids that can be modified */
        auto copy = gids;

        const bool haveDuplicate = std::unique(copy.begin(), copy.end()) != copy.end();

        if ( haveDuplicate ) {
            throw AddRuleConstructionException("Duplicate generators specified in selector");
        }
    }
}

bool Selector::Select(const GeneratorID gid) const {
    return std::find(gids.begin(), gids.end(), gid) != gids.end();
}

RewriteRule::RewriteRule(const std::map<GeneratorID, GeneratorID> mapping) :
    mapping(mapping) {

    validate();
}

void RewriteRule::validate(void) const {
    for (const auto& kv : mapping) {
        if ( kv.first == kv.second ) {
            throw AddRuleConstructionException("Circular mapping in rewrite rule");
        }
    }

    /* TODO check for deeper circular mappings */
}

GeneratorID RewriteRule::MapTo(const GeneratorID gid) const {
    if ( mapping.count(gid) == 0 ) {
        /* No mapping for this gid */
        return gid;
    }

    return mapping.at(gid);
}

AddRule::AddRule(std::string addRuleStr) {
    util::string::RemoveWhitespace(addRuleStr);
    util::string::ToLowercase(addRuleStr);

    const auto parts = util::string::Split(addRuleStr, "|");

    const auto numParts = parts.size();

    if ( numParts == 0 ) {
        throw AddRuleConstructionException("Too few components in add rule");
    }

    if ( numParts > 3 ) {
        throw AddRuleConstructionException("Too many (>3) components in add rule");
    }


    constraintSetFromString(parts[0]);

    if ( numParts >= 2 ) {
        selectorFromString(parts[1]);
    }

    if ( numParts == 3 ) {
        rewriteRuleFromString(parts[2]);
    }

}

void AddRule::constraintSetFromString(std::string constraintStr) {
    util::string::RemoveWhitespace(constraintStr);
    util::string::ToLowercase(constraintStr);

    auto parts = util::string::Split(constraintStr, ",");

    if ( parts.empty() == true ) {
        return;
    }

    const auto numParts = parts.size();

    auto constraintSet = std::make_shared<ConstraintSet>();

    for (auto& P : parts) {
        const bool invert = boost::algorithm::starts_with(P, "!");

        if ( invert == true ) {
            /* Erase exclamation mark */
            P.erase(0, 1);
        }

        enum {
            ConstraintTypeSensor,
            ConstraintTypeGenerator,
        } constraintType;

        if ( boost::algorithm::starts_with(P, "s") ) {
            constraintType = ConstraintTypeSensor;
        } else if ( boost::algorithm::starts_with(P, "g") ) {
            constraintType = ConstraintTypeGenerator;
        } else {
            throw AddRuleConstructionException("Invalid constraint type");
        }

        /* Erase first character */
        P.erase(0, 1);

        try {
            if ( constraintType == ConstraintTypeSensor ) {
                constraintSet->Add(
                    std::make_shared<SensorConstraint>(invert, boost::lexical_cast<SensorID>(P))
                );
            } else if ( constraintType == ConstraintTypeGenerator ) {
                constraintSet->Add(
                    std::make_shared<GeneratorConstraint>(invert, boost::lexical_cast<GeneratorID>(P))
                );
            }
        } catch ( boost::bad_lexical_cast ) {
            throw AddRuleConstructionException("Invalid generator ID in selector");
        }
    }

    this->constraintSet = constraintSet;
}

void AddRule::selectorFromString(const std::string selectorRuleStr) {
    const auto parts = util::string::Split(selectorRuleStr, ",");

    if ( parts.empty() == true ) {
        /* No selector specifiers, so don't bother any further */
        return;
    }

    std::vector<GeneratorID> gids;
    for (const auto& P : parts) {

        GeneratorID gid = {};
        try {
            /* Convert from string to GeneratorID */
            gid = boost::lexical_cast<GeneratorID>(P);
        } catch ( boost::bad_lexical_cast ) {
            throw AddRuleConstructionException("Invalid generator ID in selector");
        }

        gids.push_back(gid);
    }

    selector = std::make_shared<Selector>( gids );
}

void AddRule::rewriteRuleFromString(const std::string rewriteRuleStr) {
    const auto parts = util::string::Split(rewriteRuleStr, ",");

    if ( parts.empty() == true ) {
        /* No rewrite rule specifiers, so don't bother any further */
        return;
    }

    std::map<GeneratorID, GeneratorID> mapping;
    for (const auto& P : parts) {
        const auto subParts = util::string::Split(P, "=");
        if ( subParts.size() != 2 ) {
            throw AddRuleConstructionException("Single rewrite does not contain single '='");
        }

        GeneratorID from = {}, to = {};
        try {
            /* Convert from string to GeneratorID */
            from = boost::lexical_cast<GeneratorID>(subParts[0]);
            to = boost::lexical_cast<GeneratorID>(subParts[1]);
        } catch ( boost::bad_lexical_cast ) {
            throw AddRuleConstructionException("Invalid generator IDs in rewrite rule");
        }

        mapping[from] = to;
    }

    rewriteRule = std::make_shared<RewriteRule>( mapping );
}

bool AddRule::Select(const GeneratorID gid) const {
    if ( selector == std::nullopt ) {
        /* No generator IDs defined for selection */
        return false;
    }

    return (*selector)->Select(gid);
}

GeneratorID AddRule::MapTo(const GeneratorID gid) const {
    if ( rewriteRule == std::nullopt ) {
        /* No mapping defined */
        return gid;
    }

    return (*rewriteRule)->MapTo(gid);
}

} /* namespace options */
} /* namespace vfuzz */
