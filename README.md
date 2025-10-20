Here is the complete, consolidated, **working summary** of all the necessary steps, from writing the C code to achieving a stable, running DDS application orchestrated by k3s.

-----

## 🚀 Simple Container Orchestration with DDS and k3s (Working Steps)

### Step 1: Write and Test the C Code (`.c` files)

You must ensure your C applications run continuously, as containers are expected to stay running.

| File | Goal | Critical Change |
| :--- | :--- | :--- |
| **`publisher.c`** | Continuously send DDS data. | Add an **infinite loop (`while(1) { ... sleep(1); }`)** to prevent the program from exiting immediately after initialization. |
| **`subscriber.c`** | Continuously listen for DDS data. | Add a **blocking mechanism (`while(1) { sleep(10); }`)** after initialization to keep the main thread alive, allowing the DDS listener to function. |

### Step 2: Create the Docker Image

Save the `Dockerfile` in the same directory as your C files. This file must compile the code and include the necessary DDS runtime libraries.

1.  **Create `Dockerfile`:** Ensure it uses a multi-stage build (or similar) to **compile** the `.c` files into executables (`/bin/publisher` and `/bin/subscriber`).

2.  **Build the Image:** Use the exact tag you plan to use in your Kubernetes YAML.

    ```bash
    docker build -t dds-app:latest .
    ```

3.  **Verify Image:** Confirm the image is now in your local Docker store.

    ```bash
    docker images
    ```

-----

## 📦 Step 3: Configure Kubernetes (YAML Files)

Create two YAML files, ensuring they reference your local image and instruct k3s **not** to pull from a remote registry (to resolve **`ErrImagePull`**).

1.  **`publisher-deployment.yaml`** and **`subscriber-deployment.yaml`**: Ensure both files use the exact image name and the critical pull policy:

    ```yaml
    # Critical Configuration Snippet
    spec:
      containers:
      - name: publisher-container # or subscriber-container
        image: dds-app:latest
        imagePullPolicy: IfNotPresent  # CRITICAL for local image use
        command: ["/bin/publisher"] # or /bin/subscriber
    ```

-----

## 🔌 Step 4: Transfer Image to k3s Runtime (Resolving ImagePullBackOff)

Since k3s uses **containerd**, you must manually transfer the image from the Docker store to the k3s runtime. This resolves the persistent **`ErrImagePull`** and **`ImagePullBackOff`** issues.

1.  **Export the Docker Image:**

    ```bash
    docker save dds-app:latest -o dds_app_transfer.tar
    ```

2.  **Import to k3s/containerd:** Use the correct `k3s ctr` command (since `crictl image load` failed):

    ```bash
    sudo k3s ctr image import dds_app_transfer.tar
    ```

-----

## 🚀 Step 5: Deploy and Verify

Finally, apply the configuration, forcing k3s to use the newly imported image.

1.  **Clean Up Old Deployments:** Delete any previously failed or completed deployments.

    ```bash
    sudo kubectl delete deployment dds-publisher dds-subscriber-deployment
    ```

2.  **Apply New Deployments:**

    ```bash
    sudo kubectl apply -f publisher-deployment.yaml
    sudo kubectl apply -f subscriber-deployment.yaml
    ```

3.  **Verify Running Status:** Confirm the pods are stable (resolving the **`Completed`** status).

    ```bash
    sudo kubectl get pods
    # Expected Output:
    # NAME...                                 READY   STATUS    RESTARTS   AGE
    # dds-publisher-...                       1/1     Running   0          <age>
    # dds-subscriber-deployment-...           1/1     Running   0          <age>
    ```

4.  **Check DDS Communication:** View the subscriber logs to confirm it's receiving data published by the publisher.

    ```bash
    # Use the label to view logs for the running subscriber pod






    That's a great idea for a reference! Here is the summarized, end-to-end process for building a C DDS application using an IDL file and CMake with the specific fixes we identified.

## DDS Project Build Summary (C/CycloneDDS)

This summary assumes you are in your project's source directory (e.g., `/home/gururaj/Projects/soafee/simple_demo/source/`).

***

## Step 1: Create Source Files

| File | Purpose | Key Content |
| :--- | :--- | :--- |
| **`MyData.idl`** | Defines your data structure (e.g., `struct Msg { ... }`). | The DDS data type definition. |
| **`publisher.c`** | Contains the code to send data. | Includes `MyData.h`. Must use the corrected function name, e.g., `MyData_Msg__alloc()`. |
| **`subscriber.c`** | Contains the code to receive data. | Includes `MyData.h`. |
| **`CMakeLists.txt`** | The build script. | Specifies the IDL compilation, source files, and links to the DDS library. |

***

## Step 2: Configure the Build (`CMakeLists.txt`)

The core of the successful configuration lies in two fixes: correcting the `idlc_generate` keyword and directly compiling the generated C file.

| Action | Command | Rationale |
| :--- | :--- | :--- |
| **1. Find DDS** | `find_package(CycloneDDS REQUIRED)` | Locates necessary headers and libraries. |
| **2. Generate Code** | `idlc_generate(TARGET MyData_lib FILES MyData.idl)` | Runs the IDL compiler to create `MyData.c` and `MyData.h`. **FIX 1:** Uses `FILES`, not `IDL_FILES`. |
| **3. Create Executable** | `add_executable(MyPublisher publisher.c ${CMAKE_CURRENT_BINARY_DIR}/MyData.c)` | **FIX 2:** Directly includes the generated `MyData.c` source file for compilation. |
| **4. Link Libraries** | `target_link_libraries(MyPublisher PRIVATE ddsc)` | Links only the core DDS library (`ddsc`). The generated code is compiled directly, so no need to link `MyData_lib`. |

***

## Step 3: Clean, Configure, and Build

Always clean the build directory to ensure CMake picks up the updated file list.

| Command | Purpose |
| :--- | :--- |
| **Clean** | `rm -rf build` | Deletes the old build cache and files. |
| **Create Dir** | `mkdir build && cd build` | Creates and navigates into a new build directory. |
| **Configure** | `cmake ..` | Processes the `CMakeLists.txt` to create build files (Makefiles). |
| **Build** | `cmake --build .` or `make` | Compiles the sources and links the executables (`MyPublisher`, `MySubscriber`). |

***

## Step 4: Fix C Code Function Naming

If you encounter the `undefined reference` error after building, the issue is likely a **function name mismatch** in your C source files.

| Error Symptom | Correction | Rationale |
| :--- | :--- | :--- |
| `undefined reference to MyData_Msg_alloc` | **Edit `publisher.c`** and change the call from `MyData_Msg_alloc()` to `MyData_Msg__alloc()`. | CycloneDDS IDLC often uses **two underscores** (`__`) in generated C function names. |

***

## Step 5: Run the Executables

Run the executables from the **build directory** in separate terminals for them to connect and communicate.

| Terminal 1 (Subscriber) | Terminal 2 (Publisher) |
| :--- | :--- |
| `./MySubscriber` | `./MyPublisher` |
    sudo kubectl logs -f -l app=dds-subscriber
    ```
