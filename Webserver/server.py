from flask import Flask, send_file

app = Flask(__name__)

@app.route('/download')
def download_file():
    # Replace 'yourfile.exe' with the actual filename of your .exe
    path_to_file = "ransomware.exe"
    return send_file(path_to_file, as_attachment=True)

if __name__ == '__main__':
    # Run the Flask server on localhost:5000
    app.run(debug=True, port=5000)


