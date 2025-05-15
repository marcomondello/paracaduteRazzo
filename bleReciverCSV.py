import asyncio
import struct
import csv
from bleak import BleakClient, BleakScanner

# UUID della caratteristica BLE che invia il valore float da Arduino
TARGET_CHARACTERISTIC_UUID = "2A19"  # Sostituiscila con quella giusta

csv_file = open("accelerazione.csv", mode="w", newline="")
csv_writer = csv.writer(csv_file)
csv_writer.writerow(["Tempo (s)", "Accelerazione X (m/s²)"])

start_time = None

async def notification_handler(sender, data):
    """Callback che decodifica 4 byte ricevuti come float32 e salva in CSV."""
    global start_time
    if len(data) == 4:
        x = struct.unpack('<f', data)[0]
        now = asyncio.get_event_loop().time()
        if start_time is None:
            start_time = now
        timestamp = now - start_time
        print(f"[{timestamp:.2f}s] x: {x:.3f} m/s²")
        csv_writer.writerow([f"{timestamp:.2f}", f"{x:.3f}"])
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
                await asyncio.sleep(1)
        except KeyboardInterrupt:
            print("\n🛑 Interruzione. Disconnessione...")
        finally:
            await client.stop_notify(TARGET_CHARACTERISTIC_UUID)
            csv_file.close()
            print("📁 File CSV salvato.")

asyncio.run(main())
