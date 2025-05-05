import asyncio
import struct
from bleak import BleakClient, BleakScanner

# UUID della caratteristica su cui Arduino invia l'accelerazione x come float (4 byte)
TARGET_CHARACTERISTIC_UUID = "2A19"  # Assicurati che corrisponda a quella del tuo sketch Arduino

async def notification_handler(sender, data):
    """Callback che decodifica 4 byte ricevuti come float32."""
    if len(data) == 4:
        x = struct.unpack('<f', data)[0]  # '<f' = little-endian float
        print(f"x: {x:.3f} m/s²")
    else:
        print(f"Ricevuti {len(data)} byte inaspettati: {data.hex()}")

async def main():
    print("🔍 Scansione dispositivi BLE...")
    devices = await BleakScanner.discover()

    if not devices:
        print("❌ Nessun dispositivo trovato.")
        return

    for i, device in enumerate(devices):
        print(f"{i}: {device.name or 'Unknown'} - {device.address}")

    try:
        choice = int(input("Seleziona indice dispositivo: "))
        selected_device = devices[choice]
    except (ValueError, IndexError):
        print("Indice non valido.")
        return

    print(f"🔗 Connessione a {selected_device.name} ({selected_device.address})...")
    async with BleakClient(selected_device.address) as client:
        if not client.is_connected:
            print("❌ Connessione fallita.")
            return

        print("✅ Connesso. In attesa di notifiche da Arduino...")
        await client.start_notify(TARGET_CHARACTERISTIC_UUID, notification_handler)

        try:
            while True:
                await asyncio.sleep(1)  # Mantiene attivo il loop
        except KeyboardInterrupt:
            print("\n🛑 Interruzione. Disconnessione...")

        await client.stop_notify(TARGET_CHARACTERISTIC_UUID)

asyncio.run(main())
