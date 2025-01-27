#ifdef __unix__
#include "BTSockListener.h"

#include <iostream>
#include <functional>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

uint8_t register_service(uint16_t service_uuid16) {
	const char *service_name = "Object Push";
    const char *service_dsc = "Service description";
    const char *service_prov = "Service provider";

	uint8_t rfcomm_channel = 9;

    uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, obex_uuid;
    sdp_list_t *l2cap_list = 0, 
               *rfcomm_list = 0,
               *root_list = 0,
               *proto_list = 0, 
               *obex_list = 0, 
			   *profilesList = 0,
               *access_proto_list = 0;
    sdp_data_t *channel = 0; //, *psm = 0;
	sdp_profile_desc_t profile;

	uuid_t service_uuid;
	sdp_uuid16_create(&service_uuid, service_uuid16);

    sdp_record_t *record = sdp_record_alloc();
    // set the general service ID
    sdp_set_service_id( record, service_uuid );
    // set the Service class ID
    sdp_list_t service_class = {NULL, &service_uuid};
    sdp_set_service_classes( record, &service_class);

	sdp_uuid16_create(&profile.uuid, service_uuid16);
	profile.version = 0x103;
	profilesList = sdp_list_append(0, &profile);
	sdp_set_profile_descs(record, profilesList);

    // make the service record publicly browsable
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    root_list = sdp_list_append(0, &root_uuid);
    sdp_set_browse_groups( record, root_list );

    // set l2cap information
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    l2cap_list = sdp_list_append( 0, &l2cap_uuid );
    proto_list = sdp_list_append( 0, l2cap_list );

    // set rfcomm information
    sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
    rfcomm_list = sdp_list_append( 0, &rfcomm_uuid );
    sdp_list_append( rfcomm_list, channel );
    sdp_list_append( proto_list, rfcomm_list );

	// set obex information
    sdp_uuid16_create(&obex_uuid, OBEX_UUID);
    obex_list = sdp_list_append( 0, &obex_uuid );
    sdp_list_append( proto_list, obex_list );

    // attach protocol information to service record
    access_proto_list = sdp_list_append( 0, proto_list );
    sdp_set_access_protos( record, access_proto_list );

    // set the name, provider, and description
    sdp_set_info_attr(record, service_name, service_prov, service_dsc);

    // connect to the local SDP server, register the service record, and 
    // disconnect
    sdp_session_t *session = sdp_connect( BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY );
    int err = sdp_record_register(session, record, 0);
    if (err != 0) {
		perror("error sdp_record_register");
		exit(EXIT_FAILURE);
	}

    // cleanup
    //sdp_data_free( channel );
    sdp_list_free( l2cap_list, 0 );
    sdp_list_free( rfcomm_list, 0 );
    sdp_list_free( root_list, 0 );
    sdp_list_free( access_proto_list, 0 );
	sdp_list_free(profilesList, 0);

	return rfcomm_channel;
}

BTSockListener::BTSockListener() {
	server_fd = -1;
	serviceId = 0;
}

BTSockListener::~BTSockListener() {
    if (server_fd >= 0) close(server_fd);
}

void BTSockListener::bind(uint16_t id) {
    struct sockaddr_rc local_addr = {0};
    serviceId = id;

    server_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (server_fd < 0) {
        throw std::runtime_error("Failed to create socket");
    }


    local_addr.rc_family = AF_BLUETOOTH;
    local_addr.rc_bdaddr = *BDADDR_ANY;
    local_addr.rc_channel = register_service(id);

	std::cout<< local_addr.rc_channel <<'\n';

    if (::bind(server_fd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        close(server_fd);
        throw std::runtime_error("Failed to bind RFCOMM socket");
    }

    if (listen(server_fd, 10) < 0) {
        close(server_fd);
        throw std::runtime_error("Failed to listen on RFCOMM socket");
    }
}

bool BTSockListener::accept(BTSock& btsock, bool block) {
    struct sockaddr_rc client_addr = {0};
    socklen_t opt = sizeof(client_addr);

	int flags = fcntl(server_fd, F_GETFL, 0);
    if (block) {
        fcntl(server_fd, F_SETFL, flags & ~O_NONBLOCK);
    } else {
        fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);
    }
    int client_fd = ::accept(server_fd, (struct sockaddr*)&client_addr, &opt);

    if (client_fd < 0) {
        if (block || errno != EAGAIN) {
            std::cerr << "Error accepting connection" << std::endl;
        }
        return false;
    }

    btsock = BTSock(client_fd, BTAddress(client_addr.rc_bdaddr));
    return true;
}

uint16_t BTSockListener::getShortId() {
	return serviceId;
}

#endif // __unix__

