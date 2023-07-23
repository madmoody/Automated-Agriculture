#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <libcamera/libcamera.h>
#include <libcamera/camera_manager.h>
#include <libcamera/framebuffer_allocator.h>
#include <libcamera/request.h>
#include <libcamera/stream.h>

static const char* outputPath = "/path/to/your/output/image.jpg"; // Change to your desired output path

void processCompleteRequest(libcamera::Camera* camera, libcamera::Request* request)
{
    for (auto const& [stream, buffer] : request->buffers()) {
        if (buffer->planes().empty())
            continue;

        const void* data = buffer->planes()[0].map();
        const size_t dataSize = buffer->planes()[0].length();

        FILE* file = fopen(outputPath, "wb");
        if (file) {
            fwrite(data, 1, dataSize, file);
            fclose(file);
        }

        buffer->planes()[0].unmap();
    }

    request->reuse();
    camera->queueRequest(request);
}

int main()
{
    libcamera::CameraManager manager;
    manager.start();

    const char* cameraId = managercameras()[0]->id();
    libcamera::Camera* camera = manager.get(cameraId);
    if (!camera) {
        printf("Failed to obtain camera %s\n", cameraId);
        return 1;
    }

    camera->requestCompleted.connect(processCompleteRequest);

    libcamera::Stream* stream = camera->streams().at(0);

    if (camera->acquire()) {
        printf("Failed to acquire camera %s\n", cameraId);
        return 1;
    }

    libcamera::Request* request = camera->createRequest();
    if (!request) {
        printf("Failed to create request\n");
        return 1;
    }

    request->addBuffer(stream);

    if (camera->queueRequest(request)) {
        printf("Failed to queue request\n");
        return 1;
    }

    printf("Capturing image...\n");

    while (true) {
        if (camera->acquire()) {
            printf("Failed to acquire camera %s\n", cameraId);
            return 1;
        }

        if (camera->start()) {
            printf("Failed to start camera %s\n", cameraId);
            return 1;
        }

        usleep(100000); // Wait for the capture to complete (You may adjust the delay as needed)

        camera->stop();
        camera->release();
        break;
    }

    return 0;
}
