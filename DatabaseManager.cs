using Firebase;
using Firebase.Database;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class DatabaseManager : MonoBehaviour
{
    public Button buttonTrue;      
    public Button buttonFalse;     
    public Button button25; 
    public Button button50; 
    public Button button75;
    public Button button100; 
    public TMP_Text sensorInduktif; 
    public TMP_Text sensorTemperature;
    public TMP_Text sensorIR;
    public TMP_Text SUM;

    private DatabaseReference reference; // Referensi ke Firebase Realtime Database
    private int Power = 0;      // Nilai awal untuk tombol Power

    void Start()
    {
        // Pengecekan dan perbaikan dependensi Firebase
        FirebaseApp.CheckAndFixDependenciesAsync().ContinueWith(task =>
        {
            var dependencyStatus = task.Result;
            if (dependencyStatus == DependencyStatus.Available)
            {
                InitializeFirebase();
            }
            else
            {
                Debug.LogError($"Tidak dapat menyelesaikan semua dependensi Firebase: {dependencyStatus}");
            }
        });
    }

    private void InitializeFirebase()
    {
        // Inisialisasi Firebase
        FirebaseApp app = FirebaseApp.DefaultInstance;

        // Pastikan opsi Firebase tidak null
        if (app.Options == null)
        {
            Debug.LogError("FirebaseApp.Options adalah null. Pastikan konfigurasi Firebase sudah benar.");
            return;
        }

        // Set URL Realtime Database
        app.Options.DatabaseUrl = new System.Uri("https://tugas-elitia-default-rtdb.asia-southeast1.firebasedatabase.app/");
        reference = FirebaseDatabase.GetInstance(app.Options.DatabaseUrl.ToString()).RootReference;

        // Tambahkan listener tombol
        if (buttonTrue != null)
            buttonTrue.onClick.AddListener(SendButtonStateTrue);
        else
            Debug.LogError("Button True tidak diassign di Inspector!");

        if (buttonFalse != null)
            buttonFalse.onClick.AddListener(SendButtonStateFalse);
        else
            Debug.LogError("Button False tidak diassign di Inspector!");

        if (button25 != null)
            button25.onClick.AddListener(Send25Value);
        else
            Debug.LogError("Button POWER tidak diassign di Inspector!");

        if (button50 != null)
            button50.onClick.AddListener(Send50Value);
        else
            Debug.LogError("Button Decrement tidak diassign di Inspector!");

        if (button75 != null)
            button75.onClick.AddListener(Send75Value);
        else
            Debug.LogError("Button Decrement tidak diassign di Inspector!");

        if (button100 != null)
            button100.onClick.AddListener(Send100Value);
        else
            Debug.LogError("Button Decrement tidak diassign di Inspector!");

        // Memulai membaca data sensor secara realtime
        StartRealtimeListenerInduktif();
        StartRealtimeListenerIR();
        StartRealtimeListenerSUM();
        StartRealtimeListenerTemperature();
    }

    private void StartRealtimeListenerInduktif()
    {
        if (reference == null)
        {
            Debug.LogError("Firebase Database reference belum diinisialisasi.");
            return;
        }

        if (sensorInduktif == null)
        {
            Debug.LogError("Sensor Text belum diassign di Inspector!");
            return;
        }

        // Mengambil perubahan pada data sensor di Firebase
        reference.Child("TubesIoT").Child("induktif").ValueChanged += HandleValueInduktif;
    }

    private void StartRealtimeListenerTemperature()
    {
        if (reference == null)
        {
            Debug.LogError("Firebase Database reference belum diinisialisasi.");
            return;
        }

        if (sensorTemperature == null)
        {
            Debug.LogError("Sensor Text belum diassign di Inspector!");
            return;
        }

        // Mengambil perubahan pada data sensor di Firebase
        reference.Child("TubesIoT").Child("temperature").ValueChanged += HandleTemperatureChanged;
    }

    private void StartRealtimeListenerIR()
    {
        if (reference == null)
        {
            Debug.LogError("Firebase Database reference belum diinisialisasi.");
            return;
        }

        if (sensorInduktif == null)
        {
            Debug.LogError("Sensor IR belum diassign di Inspector!");
            return;
        }

        // Mengambil perubahan pada data sensor di Firebase
        reference.Child("TubesIoT").Child("IR").ValueChanged += HandleValueIR;
    }
    private void StartRealtimeListenerSUM()
    {
        if (reference == null)
        {
            Debug.LogError("Firebase Database reference belum diinisialisasi.");
            return;
        }

        if (sensorInduktif == null)
        {
            Debug.LogError("Sensor SUM belum diassign di Inspector!");
            return;
        }

        // Mendengarkan perubahan pada data sensor di Firebase
        reference.Child("TubesIoT").Child("SUM").ValueChanged += HandleValueSUM;
    }

    private void HandleValueInduktif(object sender, ValueChangedEventArgs args)
    {
        // Mengecek jika ada error pada pembacaan data
        if (args.DatabaseError != null)
        {
            Debug.LogError($"Database error: {args.DatabaseError.Message}");
            return;
        }

        // Memastikan data snapshot ada
        if (args.Snapshot == null || !args.Snapshot.Exists)
        {
            Debug.LogWarning("Snapshot data tidak ditemukan.");
            return;
        }

        // Mengambil data sensor sebagai string (JSON)
        string sensorData = args.Snapshot.GetRawJsonValue();
        Debug.Log($"Data sensor realtime diterima: {sensorData}");

        // Menampilkan data sensor pada UI TextMeshPro
        if (sensorInduktif != null)
        {
            sensorInduktif.text = $"{sensorData}";
        }
    }
    private void HandleValueIR(object sender, ValueChangedEventArgs args)
    {
        // Mengecek jika ada error pada pembacaan data
        if (args.DatabaseError != null)
        {
            Debug.LogError($"Database error: {args.DatabaseError.Message}");
            return;
        }

        // Memastikan data snapshot ada
        if (args.Snapshot == null || !args.Snapshot.Exists)
        {
            Debug.LogWarning("Snapshot data tidak ditemukan.");
            return;
        }

        // Mengambil data sensor sebagai string (JSON)
        string sensorData = args.Snapshot.GetRawJsonValue();
        Debug.Log($"Data sensor realtime diterima: {sensorData}");

        // Menampilkan data sensor pada UI TextMeshPro
        if (sensorInduktif != null)
        {
            sensorIR.text = $"{sensorData}";
        }
    }

    private void HandleValueSUM(object sender, ValueChangedEventArgs args)
    {
        // Mengecek jika ada error pada pembacaan data
        if (args.DatabaseError != null)
        {
            Debug.LogError($"Database error: {args.DatabaseError.Message}");
            return;
        }

        // Memastikan data snapshot ada
        if (args.Snapshot == null || !args.Snapshot.Exists)
        {
            Debug.LogWarning("Snapshot data tidak ditemukan.");
            return;
        }

        // Mengambil data sensor sebagai string (JSON)
        string sensorData = args.Snapshot.GetRawJsonValue();
        Debug.Log($"Data sensor realtime diterima: {sensorData}");

        // Menampilkan data sensor pada UI TextMeshPro
        if (sensorInduktif != null)
        {
            SUM.text = $"{sensorData}";
        }
    }

    private void HandleTemperatureChanged(object sender, ValueChangedEventArgs args)
    {
        // Mengecek jika ada error pada pembacaan data
        if (args.DatabaseError != null)
        {
            Debug.LogError($"Database error: {args.DatabaseError.Message}");
            return;
        }

        // Memastikan data snapshot ada
        if (args.Snapshot == null || !args.Snapshot.Exists)
        {
            Debug.LogWarning("Snapshot data tidak ditemukan.");
            return;
        }

        // Mengambil data sensor sebagai string (JSON)
        string sensorData = args.Snapshot.GetRawJsonValue();
        Debug.Log($"Data sensor realtime diterima: {sensorData}°C");

        // Menampilkan data sensor pada UI TextMeshPro
        if (sensorTemperature != null)
        {
            sensorTemperature.text = $"{sensorData}°C";
        }
    }

    public void SendButtonStateTrue()
    {
        SendButtonState("ButtonState", true);
    }

    public void SendButtonStateFalse()
    {
        SendButtonState("ButtonState", false);
    }

    private void SendButtonState(string buttonName, bool state)
    {
        ButtonState buttonState = new ButtonState(state);
        string json = JsonUtility.ToJson(buttonState);

        reference.Child(buttonName).SetRawJsonValueAsync(json)
            .ContinueWith(task =>
            {
                if (task.IsCompleted)
                {
                    Debug.Log($"Data {state} berhasil dikirim untuk {buttonName}");
                }
                else
                {
                    Debug.LogError($"Gagal mengirim data untuk {buttonName}: " + task.Exception);
                }
            });
    }

    private void Send25Value()
    {
        Power = 25; // Tambah nilai dengan 50 setiap kali tombol ditekan
        IncrementValue incrementValueData = new IncrementValue(Power);
        string json = JsonUtility.ToJson(incrementValueData);

        reference.Child("TubesIoT").Child("power").SetRawJsonValueAsync(json)
            .ContinueWith(task =>
            {
                if (task.IsCompleted)
                {
                    Debug.Log($"Nilai Power {Power} berhasil dikirim.");
                }
                else
                {
                    Debug.LogError($"Gagal mengirim nilai Power: " + task.Exception);
                }
            });
    }

    private void Send50Value()
    {
        Power = 50; // Kurangi nilai dengan 50 setiap kali tombol ditekan
        IncrementValue incrementValueData = new IncrementValue(Power);
        string json = JsonUtility.ToJson(incrementValueData);

        reference.Child("TubesIoT").Child("power").SetRawJsonValueAsync(json)
            .ContinueWith(task =>
            {
                if (task.IsCompleted)
                {
                    Debug.Log($"Nilai Power {Power} berhasil dikirim.");
                }
                else
                {
                    Debug.LogError($"Gagal mengirim nilai Power: " + task.Exception);
                }
            });
    }

    private void Send75Value()
    {
        Power = 75; // Kurangi nilai dengan 50 setiap kali tombol ditekan
        IncrementValue incrementValueData = new IncrementValue(Power);
        string json = JsonUtility.ToJson(incrementValueData);

        reference.Child("TubesIoT").Child("power").SetRawJsonValueAsync(json)
            .ContinueWith(task =>
            {
                if (task.IsCompleted)
                {
                    Debug.Log($"Nilai Power {Power} berhasil dikirim.");
                }
                else
                {
                    Debug.LogError($"Gagal mengirim nilai Power: " + task.Exception);
                }
            });
    }

    private void Send100Value()
    {
        Power = 100; // Kurangi nilai dengan 50 setiap kali tombol ditekan
        IncrementValue incrementValueData = new IncrementValue(Power);
        string json = JsonUtility.ToJson(incrementValueData);

        reference.Child("TubesIoT").Child("power").SetRawJsonValueAsync(json)
            .ContinueWith(task =>
            {
                if (task.IsCompleted)
                {
                    Debug.Log($"Nilai Power {Power} berhasil dikirim.");
                }
                else
                {
                    Debug.LogError($"Gagal mengirim nilai Power: " + task.Exception);
                }
            });
    }
}
