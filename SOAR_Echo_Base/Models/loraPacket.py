class LoraPacket:
    def __init__(self, valid_data, address, length, data_bytes, checksum, rssi, snr):
        self.valid_data = valid_data
        self.address = address
        self.length = length
        self.data_bytes = data_bytes
        self.checksum = checksum
        self.rssi = rssi
        self.snr = snr
    def to_json(self):
        return {"packet":{
            "valid_data": self.valid_data,
            "address": self.address,
            "length": self.length,
            "data_bytes": self.data_bytes,
            "checksum": self.checksum,
            "rssi": self.rssi,
            "snr": self.snr
        }}
        
        
    def __str__(self):
        return f"LORA: Valid data:{self.valid_data}, Address Bytes: {self.address}, Length Bytes: {self.length}, Data Bytes: {self.data_bytes}, Checksum: {self.checksum}, RSSI: {self.rssi}, SNR: {self.snr}"

    def __repr__(self):
        return f"Valid Data Byte: {self.valid_data}, Address Bytes: {self.address}, Length Bytes: {self.length}, Data Bytes: {self.data_bytes}, Checksum: {self.checksum}, RSSI: {self.rssi}, SNR: {self.snr}"