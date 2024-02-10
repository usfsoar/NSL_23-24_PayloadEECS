# json class for fake_serial_request.py
# {
#     "csv_file": base64 string
# }

class FakeSerialRequest:
    def __init__(self, csv_file):
        self.csv_file = csv_file