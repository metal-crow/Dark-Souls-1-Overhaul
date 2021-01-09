#include "google/cloud/storage/client.h"
#include "quickstart.h"

static google::cloud::StatusOr<google::cloud::storage::Client> client_;

bool GoogleCloudLib::CreateClient(std::string const& keyfile)
{
    namespace gcs = google::cloud::storage;

    auto creds = gcs::oauth2::CreateServiceAccountCredentialsFromJsonFilePath(keyfile);
    if (!creds) {
        return false;
    }
    client_ = google::cloud::StatusOr<gcs::Client>(gcs::Client(gcs::ClientOptions(*creds)));
    if (!client_.ok()) {
        //global::cmd_out << "Failed to create Storage Client, status=" << client.status().message() << " [" << StatusCodeToString(client.status().code()) << "]" << "\n";
        return false;
    }
    
    return true;
}

bool GoogleCloudLib::UploadFile(std::string const& bucketname, std::string const& filename, std::string const& objectname)
{
    google::cloud::StatusOr<google::cloud::storage::ObjectMetadata> metadata = client_->UploadFile(filename, bucketname, objectname);
    if (!metadata) {
        return false;
    }
    
    return true;
}