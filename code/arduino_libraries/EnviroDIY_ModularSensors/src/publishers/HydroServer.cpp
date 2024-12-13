/**
 * @file HydroServer.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the HydroServer class.
 */

#include "HydroServer.h"


// ============================================================================
//  Functions for the HydroServer data portal receivers.
// ============================================================================

// Constant values for post requests
// I want to refer to these more than once while ensuring there is only one copy
// in memory
const char* HydroServerPublisher::postEndpoint           = "/api/sensorthings/v1.1/CreateObservations";
const char* HydroServerPublisher::hydroServerHost        = "lro.hydroserver.org";
const int   HydroServerPublisher::hydroServerPort        = 80;
const char* HydroServerPublisher::acceptHeader           = "\r\nAccept: application/json";
const char* HydroServerPublisher::authorizationHeader    = "\r\nAuthorization: Basic "
const char* HydroServerPublisher::contentTypeHeader      = "\r\nContent-Type: application/json\r\n\r\n";

// HTTP Post request body json tags
const char* HydroServerPublisher::datastreamTag = "\"Datastream\":";
const char* HydroServerPublisher::iotTag        = "\"@iot.id\":";
const char* HydroServerPublisher::componentsTag = "\"components\":[\"phenomenonTime\",\"result\"],";
const char* HydroServerPublisher::dataArrayTag  = "\"dataArray\":";



// Constructors
HydroServerPublisher::HydroServerPublisher() : dataPublisher() {}
HydroServerPublisher::HydroServerPublisher(Logger& baseLogger, uint8_t sendEveryX,
                                       uint8_t sendOffset)
    : dataPublisher(baseLogger, sendEveryX, sendOffset) {}
HydroServerYPublisher::HydroServerPublisher(Logger& baseLogger, Client* inClient,
                                       uint8_t sendEveryX, uint8_t sendOffset)
    : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset) {}
HydroServerPublisher::HydroServerPublisher(Logger&     baseLogger,
                                       const char* base64Authorization,
                                       uint8_t sendEveryX, uint8_t sendOffset)
    : dataPublisher(baseLogger, sendEveryX, sendOffset) {
    setAuthorization(base64Authorization);
}
HydroServerPublisher::HydroServerPublisher(Logger& baseLogger, Client* inClient,
                                       const char* base64Authorization,
                                       uint8_t sendEveryX, uint8_t sendOffset)
    : dataPublisher(baseLogger, inClient, sendEveryX, sendOffset) {
    setAuthorization(base64Authorization);
}
// Destructor
HydroServerPublisher::~HydroServerPublisher() {}


void HydroServerPublisher::setAuthorization(const char* base64Authorization) {
    _base64Authorization = base64Authorization;
}


// Calculates how long the JSON will be
uint16_t EnviroDIYPublisher::calculateJsonSize() {
    uint16_t jsonLength = 1;   // [
    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
        jsonLength += 15;          // {"Datastream":{
        jsonLength += 10;          // "@iot.id":
        jsonLength += 1;           // "
        jsonLength += 36;          // *Datastream ID*
        jsonLength += 3;           // "},
        jsonLength += 41;          // "components":["phenomenonTime","result"],
        jsonLength += 13;          // "dataArray":[
        jsonLength += 23;          // ["*phenomenonTime*",
        jsonLength += _baseLogger->getValueStringAtI(i).length(); // *result*
        if (i == _baseLogger->getArrayVarCount() - 1) {
            jsonLength += 3;       // ]]}
        }
        else {
            jsonLength += 4;       // ]]},
        }
    }
    jsonLength += 1;               // ]

    return jsonLength;
}

// This prints a properly formatted JSON for HydroServer to an Arduino stream
void HydroServerPublisher::printSensorDataJSON(Stream* stream) {
    stream->print('[');
    for (uint8_t i = 0; i < _baseLogger->getArrayVarCount(); i++) {
        stream->print('{');
        stream->print(datastreamTag);
        stream->print('{');
        stream->print(iotTag);
        stream->print(_baseLogger->getVarUUIDAtI(i));
        stream->print('"');
        stream->print("},");
        stream->print(componentsTag);
        stream->print(dataArrayTag);
        stream->print("[[\"");
        stream->print(Logger::formatDateTime_ISO8601(Logger::markedLocalEpochTime));
        stream->print("\",");
        stream->print(_baseLogger->getValueStringAtI(i));
        if (i == _baseLogger->getArrayVarCount() - 1) {
            stream->print("]]}");
        }
        else {
            stream->print("]]},")
        }
    }
    stream->print(']');
}


// This prints a fully structured post request for HydroServer to the
// specified stream.
void HydroServerPublisher::printHydroServerRequest(Stream* stream) {
    // Stream the HTTP headers for the post request
    stream->print(postHeader);
    stream->print(postEndpoint);
    stream->print(HTTPtag);
    stream->print(hostHeader);
    stream->print(hydroServerHost);
    stream->print(acceptHeader);
    stream->print(authorizationHeader);
    stream->print(contentTypeHeader);

    // Stream the JSON itself
    printSensorDataJSON(stream);
}


// A way to begin with everything already set
void HydroServerPublisher::begin(Logger& baseLogger, Client* inClient,
                               const char* base64Authorization) {
    setAuthorization(base64Authorization);
    dataPublisher::begin(baseLogger, inClient);
}
void HydroServerPublisher::begin(Logger&     baseLogger,
                               const char* base64Authorization) {
    setAuthorization(base64Authorization);
    dataPublisher::begin(baseLogger);
}


// This utilizes an attached modem to make a TCP connection to the
// EnviroDIY/ODM2DataSharingPortal and then streams out a post request
// over that connection.
// The return is the http status code of the response.
// int16_t EnviroDIYPublisher::postDataEnviroDIY(void)
int16_t HydroServerPublisher::publishData(Client* outClient) {
    // Create a buffer for the temporary portions of the request and response
    char     tempBuffer[37] = "";
    uint16_t did_respond    = 0;

    MS_DBG(F("Outgoing JSON size:"), calculateJsonSize());

    // Open a TCP/IP connection to HydroServer
    MS_DBG(F("Connecting client"));
    MS_START_DEBUG_TIMER;
    if (outClient->connect(hydroServerHost, hydroServerPort)) {
        MS_DBG(F("Client connected after"), MS_PRINT_DEBUG_TIMER, F("ms\n"));

        // copy the initial post header into the tx buffer
        snprintf(txBuffer, sizeof(txBuffer), "%s", postHeader);
        snprintf(txBuffer + strlen(txBuffer),
                sizeof(txBuffer) - strlen(txBuffer), "%s", postEndpoint);
        snprintf(txBuffer + strlen(txBuffer),
                sizeof(txBuffer) - strlen(txBuffer), "%s", HTTPtag);

        // add the rest of the HTTP POST headers to the outgoing buffer
        // before adding each line/chunk to the outgoing buffer, we make sure
        // there is space for that line, sending out buffer if not
        if (bufferFree() < 37) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", hostHeader);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", hydroServerHost);

        if (bufferFree() < 47) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", acceptHeader);

        if (bufferFree() < 26) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s",
                 authorizationHeader);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", _base64Authorization);

        if (bufferFree() < 42) printTxBuffer(outClient);
        snprintf(txBuffer + strlen(txBuffer),
                 sizeof(txBuffer) - strlen(txBuffer), "%s", contentTypeHeader);

        // put the start of the JSON into the outgoing response_buffer
        if (bufferFree() < 1) printTxBuffer(outClient);
        txBuffer[strlen(txBuffer)] = '[';
        for (uint8_t y = 0; y < _baseLogger->getArrayVarCount(); y++) {
            if (bufferFree() < 1) printTxBuffer(outClient);
            txBuffer[strlen(txBuffer)] = '{';

            if (bufferFree() < 13) printTxBuffer(outClient);
            snprintf(txBuffer + strlen(txBuffer),
                    sizeof(txBuffer) - strlen(txBuffer), "%s", dataStreamTag);

            if (bufferFree() < 10) printTxBuffer(outClient);
            txBuffer[strlen(txBuffer)] = '{';
            snprintf(txBuffer + strlen(txBuffer),
                sizeof(txBuffer) - strlen(txBuffer), "%s", iotTag);

            if (bufferFree() < 1) printTxBuffer(outClient);
            txBuffer[strlen(txBuffer)] = '"';

            if (bufferFree() < 37) printTxBuffer(outClient);
            _baseLogger->getVarUUIDAtI(y).toCharArray(tempBuffer, 37);
            snprintf(txBuffer + strlen(txBuffer),
                sizeof(txBuffer) - strlen(txBuffer), "%s", tempBuffer);

            if (bufferFree() < 1) printTxBuffer(outClient);
            txBuffer[strlen(txBuffer)] = "\"},";

            if (bufferFree() < 42) printTxBuffer(outClient);
            snprintf(txBuffer + strlen(txBuffer),
                sizeof(txBuffer) - strlen(txBuffer), "%s", componentsTag);

            if (bufferFree() < 12) printTxBuffer(outClient);
            snprintf(txBuffer + strlen(txBuffer),
                sizeof(txBuffer) - strlen(txBuffer), "%s", dataArrayTag);

            if (bufferFree() < 3) printTxBuffer(outClient);
            txBuffer[strlen(txBuffer)] = "[[\"";

            if (bufferFree() < 37) printTxBuffer(outClient);
            Logger::formatDateTime_ISO8601(Logger::markedLocalEpochTime).toCharArray(tempBuffer, 37);
            snprintf(txBuffer + strlen(txBuffer),
                sizeof(txBuffer) - strlen(txBuffer), "%s", tempBuffer);

            if (bufferFree() < 2) printTxBuffer(outClient);
            txBuffer[strlen(txBuffer)] = "\",";

            if (bufferFree() < 37) printTxBuffer(outClient);
            _baseLogger->getValueStringAtI(y).toCharArray(tempBuffer, 37);
            snprintf(txBuffer + strlen(txBuffer),
                sizeof(txBuffer) - strlen(txBuffer), "%s", tempBuffer);
            
            if (bufferFree() < 3) printTxBuffer(outClient);
            txBuffer[strlen(txBuffer)] = "]]}";

            // If this is not the last variable
            if (y != _baseLogger->getArrayVarCount() - 1) {
                if (bufferFree() < 1) printTxBuffer(outClient);
                txBuffer[strlen(txBuffer)] = ',';
            }
        }
        if (bufferFree() < 1) printTxBuffer(outClient);
        txBuffer[strlen(txBuffer)] = ']';

        // Send out the finished request (or the last unsent section of it)
        printTxBuffer(outClient, true);

        // Wait 10 seconds for a response from the server
        uint32_t start = millis();
        while ((millis() - start) < 10000L && outClient->available() < 12) {
            delay(10);
        }

        // Read only the first 12 characters of the response
        // We're only reading as far as the http code, anything beyond that
        // we don't care about.
        did_respond = outClient->readBytes(tempBuffer, 12);

        // Close the TCP/IP connection
        MS_DBG(F("Stopping client"));
        MS_RESET_DEBUG_TIMER;
        outClient->stop();
        MS_DBG(F("Client stopped after"), MS_PRINT_DEBUG_TIMER, F("ms"));
    } else {
        PRINTOUT(F("\n -- Unable to Establish Connection to EnviroDIY Data "
                   "Portal --"));
    }

    // Process the HTTP response
    int16_t responseCode = 0;
    if (did_respond > 0) {
        char responseCode_char[4];
        for (uint8_t i = 0; i < 3; i++) {
            responseCode_char[i] = tempBuffer[i + 9];
        }
        responseCode = atoi(responseCode_char);
    } else {
        responseCode = 504;
    }

    PRINTOUT(F("-- Response Code --"));
    PRINTOUT(responseCode);

    return responseCode;
}
