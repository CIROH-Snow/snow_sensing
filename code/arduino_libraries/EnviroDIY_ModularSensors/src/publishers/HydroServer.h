/**
 * @file HydroServer.h
 * 
 * This file and the associated cpp main file were created by Braedon Dority
 * by modifying the publisher files created by the Stroud Water Research Center
 * for their Monitor My Watershed server.
 * 
 * For more information contact Jeff Horsburgh <jeff.horsburgh@usu.edu>
 * 
 * This is part of the CIROH project: Advancing Snow Observations
 * 
 * The original author for the Monitor My Watershed publisher is
 * Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * 
 * The following is their original copyright:
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the HydroServerPublisher subclass of dataPublisher for
 * publishing data to the LRO HydroServer data portal at
 * http://lro.hydroserver.org
 * Note that this is for the Logan River Observatory's (LRO) instance of
 * HydroServer. If you are going to use a different instance, you will need
 * to change the hydroServerHost string found in the cpp file.
 */

// Header Guards
#ifndef SRC_PUBLISHERS_HYDROSERVERPUBLISHER_H_
#define SRC_PUBLISHERS_HYDROSERVERPUBLISHER_H_

// Debugging Statement
// #define MS_HYDROSERVERPUBLISHER_DEBUG

#ifdef MS_HYDROSERVERPUBLISHER_DEBUG
#define MS_DEBUGGING_STD "HydroServerPublisher"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "dataPublisherBase.h"


// ============================================================================
//  Functions for the HydroServer data portal receivers.
// ============================================================================
/**
 * @brief The HydroServerPublisher subclass of dataPublisher for publishing data
 * to the HydroServer data portal at
 * http://lro.hydroserver.org
 *
 * @ingroup the_publishers
 */
class HydroServerPublisher : public dataPublisher {
 public:
    // Constructors
    /**
     * @brief Construct a new HydroServer Publisher object with no members set.
     */
    HydroServerPublisher();
    /**
     * @brief Construct a new HydroServer Publisher object
     *
     * @note If a client is never specified, the publisher will attempt to
     * create and use a client on a LoggerModem instance tied to the attached
     * logger.
     *
     * @param baseLogger The logger supplying the data to be published
     * @param sendEveryX Currently unimplemented, intended for future use to
     * enable caching and bulk publishing
     * @param sendOffset Currently unimplemented, intended for future use to
     * enable publishing data at a time slightly delayed from when it is
     * collected
     *
     * @note It is possible (though very unlikey) that using this constructor
     * could cause errors if the compiler attempts to initialize the publisher
     * instance before the logger instance.  If you suspect you are seeing that
     * issue, use the null constructor and a populated begin(...) within your
     * set-up function.
     */
    explicit HydroServerPublisher(Logger& baseLogger, uint8_t sendEveryX = 1,
                                uint8_t sendOffset = 0);
    /**
     * @brief Construct a new EnviroDIY Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param sendEveryX Currently unimplemented, intended for future use to
     * enable caching and bulk publishing
     * @param sendOffset Currently unimplemented, intended for future use to
     * enable publishing data at a time slightly delayed from when it is
     * collected
     *
     * @note It is possible (though very unlikey) that using this constructor
     * could cause errors if the compiler attempts to initialize the publisher
     * instance before the logger instance.  If you suspect you are seeing that
     * issue, use the null constructor and a populated begin(...) within your
     * set-up function.
     */
    HydroServerPublisher(Logger& baseLogger, Client* inClient,
                       uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    /**
     * @brief Construct a new EnviroDIY Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param base64Authorization The <username:password> encoded as base64 for
     * the site on the HydroServer data portal.
     * @param sendEveryX Currently unimplemented, intended for future use to
     * enable caching and bulk publishing
     * @param sendOffset Currently unimplemented, intended for future use to
     * enable publishing data at a time slightly delayed from when it is
     * collected
     */
    HydroServerPublisher(Logger& baseLogger, const char* base64Authorization,
                         uint8_t sendEveryX = 1, uint8_t sendOffset = 0);
    /**
     * @brief Construct a new EnviroDIY Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param base64Authorization The <username:password> encoded as base64 for
     * the site on the HydroServer data portal.
     * @param sendEveryX Currently unimplemented, intended for future use to
     * enable caching and bulk publishing
     * @param sendOffset Currently unimplemented, intended for future use to
     * enable publishing data at a time slightly delayed from when it is
     * collected
     */
    HydroServerPublisher(Logger& baseLogger, Client* inClient,
                       const char* base64Authorization,
                       uint8_t sendEveryX = 1,
                       uint8_t sendOffset = 0);
    /**
     * @brief Destroy the EnviroDIY Publisher object
     */
    virtual ~HydroServerPublisher();

    // Returns the data destination
    String getEndpoint(void) override {
        return String(hydroServerHost);
    }

    // Adds the site authorization
    /**
     * @brief Set the site authorization
     *
     * @param base64Authorization The <username:password> encoded as base64
     */
    void setAuthorization(const char* base64Authorization);

    /**
     * @brief Calculates how long the outgoing JSON will be
     *
     * @return uint16_t The number of characters in the JSON object.
     */
    uint16_t calculateJsonSize();
    // /**
    //  * @brief Calculates how long the full post request will be, including
    //  * headers
    //  *
    //  * @return uint16_t The length of the full request including HTTP
    //  headers.
    //  */
    // uint16_t calculatePostSize();

    /**
     * @brief This generates a properly formatted JSON for HydroServer's CreateObservations
     * API endpoint and prints it to the input Arduino stream object.
     *
     * @param stream The Arduino stream to write out the JSON to.
     */
    void printSensorDataJSON(Stream* stream);

    /**
     * @brief This prints a fully structured post request for HydroServer to the specified stream.
     *
     * @param stream The Arduino stream to write out the request to.
     */
    void printHydroServerRequest(Stream* stream);

    // A way to begin with everything already set
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger, Client* inClient)
     * @param inClient An Arduino client instance to use to print data to.
     * @param base64Authorization The <username:password> encoded as base64 for
     * the site on the HydroServer data portal.
     */
    void begin(Logger& baseLogger, Client* inClient,
               const char* base64Authorization);
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger)
     * @param base64Authorization The <username:password> encoded as base64 for
     * the site on the HydroServer data portal.
     */
    void begin(Logger& baseLogger, const char* base64Authorization);

    // NOTE: This is commented code taken from the EnviroDIY MMW publisher's header file
    // I left it in here it case it was to be used or added in some way in the future
    // 
    // int16_t postDataEnviroDIY(void);
    /**
     * @brief Utilize an attached modem to open a a TCP connection to the
     * EnviroDIY/ODM2DataSharingPortal and then stream out a post request over
     * that connection.
     *
     * This depends on an internet connection already having been made and a
     * client being available.
     *
     * @param outClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @return **int16_t** The http status code of the response.
     */
    int16_t publishData(Client* outClient) override;

 protected:
    /**
     * @anchor hydroserver_post_vars
     * @name Portions of the POST request to HydroServer
     *
     * @{
     */
    static const char* postEndpoint;   ///< The endpoint
    static const char* hydroServerHost;  ///< The host name
    static const int   hydroServerPort;  ///< The host port
    static const char* acceptHeader;    ///< The token header text
    // static const char *cacheHeader;  ///< The cache header text
    // static const char *connectionHeader;  ///< The keep alive header text
    static const char* authorizationHeader;  ///< The authorization header text
    static const char* contentTypeHeader;    ///< The content type header text
    /**@}*/

 private:
    // Tokens and UUID's for EnviroDIY
    const char* _base64Authorization = nullptr;
};

#endif  // SRC_PUBLISHERS_HYDROSERVERPUBLISHER_H_
