# Matter Telink Lock with Aliro Example Application

The Telink Aliro Minimal Lock Example demonstrates a Matter door lock with
Aliro credential provisioning and NFC access on `tl3238x`. The application is
commissioned over Matter BLE, operates on a Thread network, and uses a CLRC663
NFC frontend for Aliro standard transactions.

Aliro BLE-only standard RKE build profiles are also available for `tl7218x`.
The legacy-advertising profile builds for TL7218X; Wallet transactions and
BLE/Thread coexistence still require runtime validation.

Matter owns the device lifecycle, BLE commissioning, Thread networking, and
Matter persistence. The Aliro SDK is linked as a library and uses the same
Zephyr Bluetooth host as Matter; it does not initialize or own a second BLE
stack.

## Supported devices

| Board/SoC | Build target | NFC frontend | Zephyr Board Info |
| :-------- | :----------- | :----------- | :---------------- |
| TL3238X | `tl3238x` | CLRC663 | [TL3238X](https://github.com/telink-semi/zephyr/tree/telink_aliro_baza_zephyr_4.1.0/boards/telink/tl323x) |
| TL7218X | `tl7218x` | Not used by BLE profile | [TL7218X](https://github.com/telink-semi/zephyr/tree/feat-concurrent/boards/telink/tl721x) |

This application has been tested with Telink Zephyr revision
`69f4e4ebf0f607c1808e6f5ff7e91c6f6c531a29`, Telink HAL revision
`ce77c8f74d7e99a75d755dd4c3b43c859cb00b1b`, and Zephyr SDK 0.17.0.
These revisions describe the validated TL3238X NFC configuration. The TL7218X
BLE profile requires Telink's concurrent BLE/Thread Zephyr branch and its
matching HAL revision.

## Implemented functionality

- Matter commissioning over BLE and normal operation over Thread.
- Two Matter fabrics with four access-control entries per fabric.
- A Door Lock endpoint with PIN, COTA, User, and Aliro Provisioning features.
- Six users, two PIN credentials, three Aliro issuer keys, and six combined
  evictable or non-evictable Aliro endpoint keys.
- CLRC663 low-power card detection using a GPIO interrupt, followed by an Aliro
  standard NFC transaction.
- Authorization of the authenticated Aliro endpoint key against an occupied
  Matter lock user before accepting the requested lock action.
- A simulated two-second lock actuator controlled by Matter, NFC, or a button.

Apple Home has been used to commission the device, provision a Home Key in Apple Wallet, and
unlock the simulated Matter lock through the CLRC663 reader.

## Apple Home commissioning and NFC unlock flow

The following sequence is intended to document the tested Apple Home flow from
initial Matter commissioning through an Aliro NFC unlock.

### 1. Add the Matter accessory

Open the Apple Home accessory setup flow and scan the Matter QR code, or enter
the manual setup code. At this stage the lock is discovered through Matter BLE
commissioning.

|  |  |
| :---: | :---: |
| ![Left](./images/photo_1.jpg) | ![Right](./images/photo_2.jpg) |

### 2. Commission the lock, finish accessory and Home Key setup

Apple Home establishes the commissioning session, provisions the Thread
operational credentials, and adds the lock to the selected home.

|  |  |  |
| :---: | :---: | :---: |
| ![Left](./images/photo_3.jpg) | ![Center](./images/photo_4.jpg) | ![Right](./images/photo_5.jpg) |

Complete the accessory name and room selection. Apple Home then provisions the
Aliro reader configuration, issuer credentials, endpoint credentials, and lock
users required for Home Key operation.
|  |  |
| :---: | :---: |
| ![Left](./images/photo_6.jpg) | ![Right](./images/photo_7.jpg) |

### 3. Confirm the commissioned lock

Confirm that the lock appears in Apple Home and is reachable on the network.
|  |  |
| :---: | :---: |
| ![Left](./images/photo_8.jpg) | ![Right](./images/photo_9.jpg) |

### 4. Unlock with the Home Key in Apple Wallet over NFC

Present the iPhone containing the Home Key to the CLRC663 NFC reader. This is an Aliro NFC transaction; Matter BLE is not used. The application authenticates the endpoint key, verifies that it belongs to an occupied Matter lock user, performs the requested unlock action, and reports the updated Door Lock state over Thread. Apple Wallet confirms the transaction, and Apple Home displays the lock as unlocked.

|  |  |
| :---: | :---: |
| ![Left](./images/photo_10.jpg) | ![Right](./images/photo_11.jpg) |

## Build and flash

Build flow of this application doesn't change much from any other Telink example apps.
Initial Matter/Zephyr environment setup is mostly the same as in [Telink Developer's Guide](https://doc.telink-semi.cn/doc/en/software/res/sdk/matter/telink_matter_developer_guide_en/).
You just need to checkout specific branch of Zephyr using current Matter revision.

1. Prepare the connectedhomeip and Telink Zephyr build environment. The Zephyr
   workspace must include the `tl3238x` board and the Telink HAL revisions noted
   above.

2. Activate the Matter build environment from the connectedhomeip root:

    ```bash
    source scripts/activate.sh -p all,telink
    ```

3. Build the application from `examples/lock-app/telink-aliro-minimal`:

    ```bash
    west build -p always -b tl3238x
    ```

   CMake downloads the Aliro SDK archive configured by
   `TELINK_ALIRO_SDK_URL`, extracts it under `build/_deps`, and links the
   `Telink::Aliro` target. The resulting image is `build/zephyr/merged.bin`.

   In case you want to use another published SDK archive:

    ```bash
    west build -p always -b tl3238x -- \
      -DTELINK_ALIRO_SDK_URL=https://server/path/telink-aliro-sdk.tar.gz
    ```

   To build against a local Aliro source or SDK checkout without downloading an
   archive:

    ```bash
    west build -p always -b tl3238x -- \
      -DFETCHCONTENT_SOURCE_DIR_TELINK_ALIRO=/absolute/path/to/aliro
    ```

   For TL7218X BLE bring-up with a single advertiser, use the concurrent
   BLE/Thread Zephyr and HAL revisions, then add `prj_ble_legacy.conf`:

    ```bash
    west build -p always -b tl7218x -d build-tl7218x-ble-legacy -- \
      -DEXTRA_CONF_FILE=prj_ble_legacy.conf \
      -DFETCHCONTENT_SOURCE_DIR_TELINK_ALIRO=/absolute/path/to/aliro
    ```

   Use the local Aliro source checkout containing these transport changes;
   the published SDK archive has not been updated by this change.

   The separate-advertiser profile additionally requires a controller library
   built for two peripheral connections and extended advertising:

    ```bash
    west build -p always -b tl7218x -- \
      -DEXTRA_CONF_FILE=prj_ble.conf \
      -DFETCHCONTENT_SOURCE_DIR_TELINK_ALIRO=/absolute/path/to/aliro
    ```

4. Flash the generated `merged.bin` using the TL3238X flashing procedure. The
   current TL3238X Zephyr board documentation does not enable a `west flash`
   runner.

## Hardware connections

The application devicetree overlay configures the CLRC663 as follows:

| CLRC663 signal | TL3238X pin |
| :------------- | :---------- |
| SPI chip select | PE4 |
| SPI clock | PE5 |
| SPI MISO | PE6 |
| SPI MOSI | PE7 |
| IRQ | PA5 |
| Reset | PA6 |

Power and ground must match the CLRC663 board being used.

## Usage

### UART

The Zephyr console uses UART0 at 115200 baud, 8 data bits, no parity, and one
stop bit.

| Signal | TL3238X pin |
| :----- | :---------- |
| TX | PB2 |
| RX | PB0 |
| GND | GND |

### Buttons

| Name | Function | Description |
| :--- | :------- | :---------- |
| User KEY1 | Factory reset | Press three times within three seconds to erase the commissioned Matter state. |
| User KEY2 | Lock control | Toggle the simulated lock between locked and unlocked. |

### LEDs

| LED | Function | Description |
| :-- | :------- | :---------- |
| White | Matter network status | Short pulse while uncommissioned, fast blink while joining, and long pulse while attached to Thread. |
| Green | Lock state | On when locked, off when unlocked, and fast blink while the actuator is moving. |

### Commission with CHIP Tool

Build the [CHIP Tool](../../chip-tool/README.md), then commission the device over
BLE with a Thread operational dataset:

```bash
${CHIP_TOOL_DIR}/chip-tool pairing ble-thread \
  ${NODE_ID} hex:${THREAD_DATASET} ${PIN_CODE} ${DISCRIMINATOR}
```

Lock, unlock, or read the lock state on endpoint 1:

```bash
${CHIP_TOOL_DIR}/chip-tool doorlock lock-door ${NODE_ID} 1
${CHIP_TOOL_DIR}/chip-tool doorlock unlock-door ${NODE_ID} 1
${CHIP_TOOL_DIR}/chip-tool doorlock read lock-state ${NODE_ID} 1
```

### Aliro NFC access

An ecosystem must first commission the Matter device and provision the Aliro
reader configuration, issuer key, endpoint key, and corresponding lock user.
Until the reader configuration is received, NFC transactions are rejected.

After provisioning, present the matching NFC credential to the CLRC663 reader.
The application accepts the requested lock action only when the transaction is
authenticated and its endpoint key belongs to an occupied Matter user.

### Aliro BLE bring-up profile

Use `prj_ble_legacy.conf` while the controller's extended-advertising support
is being resolved. It reserves one peripheral connection and uses Matter's
BLE advertising arbiter to share the legacy advertiser. Matter commissioning
has higher priority; after its advertising request is removed, Aliro advertises
on identity 1 while Matter operates over Thread. Reopening BLE commissioning
temporarily preempts Aliro advertising. Existing BLE connections must finish
before the single connection slot can be reused. The arbiter resumes advertising
when the connection object is released.

Both BLE profiles disable bondable mode, keeping Matter on identity 0. Without
this setting, Zephyr's Matter integration selects identity 1 and collides with
Aliro. Matter's connection callbacks also filter by identity.

The `prj_ble.conf` overlay keeps Matter BLE commissioning enabled and adds an
Aliro BLE-only RKE peripheral to the same Zephyr Bluetooth host. Matter uses
Bluetooth identity 0; Aliro creates identity 1 and a dedicated connectable
extended advertising set. The profile reserves two BLE connections so a Matter
commissioning connection and an Aliro connection can coexist at host level.

Aliro advertising starts after Matter provisions the Aliro reader
configuration. The Aliro GATT service negotiates the protocol version before
accepting its dynamic LE L2CAP channel, which carries the Aliro APDU exchange.
The initiation message supplies the exact A5 template used in authentication
key derivation. The transport queues complete incoming messages, accepts SDUs
up to 1028 bytes, waits for outgoing status completion before disconnecting,
and isolates transactions across disconnects. The reader authenticates the
provisioned endpoint key before processing the encrypted RKE lock/unlock request.
The app reports BUSY while the action is queued and waits for the simulated
actuator's final state. Fast transactions and BLE + UWB are disabled; step-up
validation is outside this batch.

The concurrent platform revisions used for build checks are Zephyr
`1a8fc1a674eb50e233a66013aa8e3ee2148d7d65`, HAL
`4e0ba44314a0da44bb76aac6953c969fd4a2eb7f`, and controller SDK
`53eb98b32ea79ed7ab38f5daabde0a78a7880cd9`. The latter's
`lib_zephyr_tl721x_concurrent.a` omits the extended-advertising implementation;
its compiler definitions also select one peripheral connection. Use the legacy
profile with this library. The separate-advertiser profile needs matching
controller binaries and compiler definitions with those capabilities enabled.

The legacy profile was compiled and linked with the revisions above. The Aliro
native simulator suites pass 12 transport and advertising checks; see
`tests/README.md` in the Aliro source checkout. These checks do not substitute
for a phone transaction on hardware.

On the board, first verify Matter commissioning and Aliro reader/endpoint
provisioning. Then check for service UUID `FFF2`, GATT version negotiation,
Initiate Access Protocol RKE, AUTH0/AUTH1, encrypted AP completion, encrypted RKE,
and final reader status. Confirm both the phone result and the lock state over
Thread. Repeat after disconnecting mid-transaction and after reopening a
commissioning window. Apple Home/Wallet BLE interoperability is not yet validated.

## Current limitations

- The default and validated profile is Aliro NFC standard transaction on
  TL3238X. The TL7218X legacy BLE-only profile has passed a firmware build;
  Aliro Wallet BLE transactions have not yet been validated.
- BLE advertising uses the Aliro no-UTC expiry value `0xFFFFFFFF` and the
  controller-reported transmit power. The group resolving key is still a
  development placeholder. Reader discovery by a phone that filters on its
  provisioned GRK remains unresolved; the BLE-only Matter feature configuration
  does not enable the BLE+UWB feature that provisions that key.
- The BLE profile depends on Telink's concurrent BLE/Thread Zephyr branch and
  matching HAL/controller library. Matter commissioning plus Aliro BLE runtime
  coexistence is not yet proven by this application.
- Aliro BLE + UWB, expedited transactions, and keyslot credentials are not
  included.
- Aliro issuer keys, endpoint keys, and reader configuration are stored in RAM
  only and are lost on reboot. Matter settings remain persistent.
- Aliro transaction-control persistence is disabled because the standalone SDK
  NVS backend targets the same flash storage used by Matter. A Matter-owned
  storage adapter is still required.
- Power management and Matter OTA Requestor support are disabled.
- The lock actuator is simulated; no physical bolt or door-position sensor is
  controlled by this application.
- Factory data is disabled and development commissioning credentials are used.
  This is not a certification or production configuration.

The TL3238X application overlay exposes 128 KiB of retained RAM and 32 KiB of
non-retained instruction RAM as separate linker regions. The Aliro NFC thread
currently uses a provisional 4 KiB stack; runtime stack and heap high-water
measurements are still required.

The default SDK URL currently identifies a test archive and is not accompanied
by a content hash. A public release should use an immutable, versioned archive
name and pin its contents before this build is treated as reproducible.
