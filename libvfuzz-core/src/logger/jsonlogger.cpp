#include <logger/jsonlogger.h>
#include <util/io.h>
#include <util/string/string.h>
#include <boost/property_tree/json_parser.hpp>

namespace vfuzz {

JSONLogger::JSONLogger(const std::string outputPath) :
    Logger(), outputPath(outputPath)
{
}

boost::property_tree::ptree JSONLogger::serializeSensorUpdateDataSingle(const SensorUpdateData * sensorUpdateData) const
{
    boost::property_tree::ptree out;

    out.put("ID", sensorUpdateData->ID());
    out.put("name", sensorUpdateData->Name());
    out.put("from", sensorUpdateData->From());
    out.put("to", sensorUpdateData->To());

    return out;
}

boost::property_tree::ptree JSONLogger::serializeInput(const std::shared_ptr<container::Input> input) const
{
    boost::property_tree::ptree out;

    const auto size = input->Size();

    for (size_t i = 0; i < size; i++) {
        const auto curChunk = input->Get(i);
        boost::property_tree::ptree asBase64;
        asBase64.put("", util::string::ToBase64(curChunk.data(), curChunk.size()));
        out.push_back(std::make_pair("", asBase64));
    }

    return out;
}
boost::property_tree::ptree JSONLogger::serializeInputCluster(const container::InputCluster* inputCluster) const
{
    boost::property_tree::ptree out;

    const auto GIDs = inputCluster->GetGIDS();

    for ( const auto& gid : GIDs ) {
        const auto curInput = inputCluster->GetUnsafe(gid);
        const auto inputAsPtree = serializeInput(curInput);
        out.add_child(std::to_string(gid), inputAsPtree);
    }

    return out;
}

void JSONLogger::flush(void) const
{
    boost::property_tree::ptree out;

    if ( queue_sensorUpdateData.empty() == true ) {
        return;
    }

    boost::property_tree::ptree sensorUpdateDataArray;
    {
        for (const auto& sud : queue_sensorUpdateData ) {
            const auto sensorUpdateDataSingleAsPtree = serializeSensorUpdateDataSingle(sud);
            sensorUpdateDataArray.push_back(std::make_pair("", sensorUpdateDataSingleAsPtree));
        }
    }

    boost::property_tree::ptree inputClustersArray;
    {
        for (const auto& ic : queue_inputClusters ) {
            const auto inputClusterAsPtree = serializeInputCluster(ic);
            inputClustersArray.push_back(std::make_pair("", inputClusterAsPtree));
        }
    }

    out.put("numexec", std::to_string(getNumExecs()));
    out.put("corpussize", std::to_string(getCorpusSize()));
    out.add_child("sensordata", sensorUpdateDataArray);
    out.add_child("inputclusters", inputClustersArray);

    const auto outputFilename = util::DirPlusFile(outputPath, std::to_string(getNumExecs()));
    boost::property_tree::write_json(outputFilename, out);
    /*
    std::ostringstream oss;
    boost::property_tree::write_json(oss, out);
    std::cout << oss.str() << std::endl;
    */
}

} /* namespace vfuzz */
