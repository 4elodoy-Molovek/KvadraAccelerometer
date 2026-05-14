package com.kvadra.accelerometer

import android.os.Bundle
import android.util.Log
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import java.io.IOException

class MainActivity : ComponentActivity() {

    private val nodeA = NodeAService()
    private val nodeB = NodeBService()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContent {
            MaterialTheme {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    AxeloControlPanel()
                }
            }
        }
    }

    @Composable
    fun AxeloControlPanel() {
        // Состояния UI
        var ipAddress by remember { mutableStateOf("192.168.1.100:50051") }
        var isNodeARunning by remember { mutableStateOf(false) }
        var isNodeBRunning by remember { mutableStateOf(false) }

        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(24.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center
        ) {
            Text(
                text = "Axelo Transmitter",
                style = MaterialTheme.typography.headlineMedium,
                fontWeight = FontWeight.Bold,
                modifier = Modifier.padding(bottom = 32.dp)
            )

            OutlinedTextField(
                value = ipAddress,
                onValueChange = { ipAddress = it },
                label = { Text("IP сервера и порт") },
                singleLine = true,
                enabled = !(isNodeARunning || isNodeBRunning),
                modifier = Modifier.fillMaxWidth()
            )

            Spacer(modifier = Modifier.height(32.dp))

            // --- Блок управления Узлом А (Передатчик) ---
            Card(
                modifier = Modifier.fillMaxWidth(),
                elevation = CardDefaults.cardElevation(defaultElevation = 4.dp)
            ) {
                Column(modifier = Modifier.padding(16.dp)) {
                    Text("Узел А (Отправка данных)", fontWeight = FontWeight.Bold)
                    Spacer(modifier = Modifier.height(8.dp))
                    Button(
                        onClick = {
                            if (isNodeARunning) {
                                stopNodeA()
                                isNodeARunning = false
                            } else {
                                val success = startNodeA(ipAddress)
                                if (success) isNodeARunning = true
                            }
                        },
                        modifier = Modifier.fillMaxWidth(),
                        colors = ButtonDefaults.buttonColors(
                            containerColor = if (isNodeARunning) MaterialTheme.colorScheme.error else MaterialTheme.colorScheme.primary
                        )
                    ) {
                        Text(if (isNodeARunning) "Остановить Узел А" else "Запустить Узел А")
                    }
                }
            }

            Spacer(modifier = Modifier.height(16.dp))

            Card(
                modifier = Modifier.fillMaxWidth(),
                elevation = CardDefaults.cardElevation(defaultElevation = 4.dp)
            ) {
                Column(modifier = Modifier.padding(16.dp)) {
                    Text("Узел B (Получение статусов)", fontWeight = FontWeight.Bold)
                    Spacer(modifier = Modifier.height(8.dp))
                    Button(
                        onClick = {
                            if (isNodeBRunning) {
                                stopNodeB()
                                isNodeBRunning = false
                            } else {
                                val success = startNodeB(ipAddress)
                                if (success) isNodeBRunning = true
                            }
                        },
                        modifier = Modifier.fillMaxWidth(),
                        colors = ButtonDefaults.buttonColors(
                            containerColor = if (isNodeBRunning) MaterialTheme.colorScheme.error else MaterialTheme.colorScheme.secondary
                        )
                    ) {
                        Text(if (isNodeBRunning) "Остановить Узел B" else "Запустить Узел B")
                    }
                }
            }
        }
    }

    private fun readAssetFile(fileName: String): String {
        return try {
            assets.open(fileName).bufferedReader().use { it.readText() }
        } catch (e: IOException) {
            Log.e("MainActivity", "Ошибка чтения файла $fileName из assets", e)
            ""
        }
    }

    private fun startNodeA(targetIp: String): Boolean {
        Log.d("MainActivity", "Запуск Узла А (mTLS) на IP: $targetIp")

        val caCert = readAssetFile("ca.crt")
        val clientCert = readAssetFile("client_a.crt")
        val clientKey = readAssetFile("client_a.key")
        val apiKey = "super-secret-key"

        if (caCert.isEmpty() || clientCert.isEmpty() || clientKey.isEmpty()) {
            Toast.makeText(this, "Не удалось загрузить серты для Узла А", Toast.LENGTH_LONG).show()
            return false
        }

        val logPath = applicationContext.filesDir.absolutePath + "/accel_module.log"

        val isStarted = nodeA.startNativeClient(targetIp, apiKey, caCert, clientCert, clientKey, logPath)

        if (isStarted) {
            Toast.makeText(this, "Узел А запущен", Toast.LENGTH_SHORT).show()
        } else {
            Toast.makeText(this, "Ошибка запуска Узла А", Toast.LENGTH_LONG).show()
        }
        return isStarted
    }

    private fun stopNodeA() {
        Log.d("MainActivity", "Остановка Узла А")
        nodeA.stopNativeClient()
        Toast.makeText(this, "Узел А остановлен", Toast.LENGTH_SHORT).show()
    }

    private fun startNodeB(targetIp: String): Boolean {
        Log.d("MainActivity", "Запуск Узла B (mTLS) на IP: $targetIp")

        val caCert = readAssetFile("ca.crt")
        val clientCert = readAssetFile("client_b.crt")
        val clientKey = readAssetFile("client_b.key")
        val apiKey = "super-secret-key"

        if (caCert.isEmpty() || clientCert.isEmpty() || clientKey.isEmpty()) {
            Toast.makeText(this, "Не удалось загрузить серты для Узла B", Toast.LENGTH_LONG).show()
            return false
        }

        // Обрати внимание: сигнатура NodeBService не требует logPath
        val isStarted = nodeB.startNativeProcessor(targetIp, apiKey, caCert, clientCert, clientKey)

        if (isStarted) {
            Toast.makeText(this, "Узел B запущен", Toast.LENGTH_SHORT).show()
        } else {
            Toast.makeText(this, "Ошибка запуска Узла B", Toast.LENGTH_LONG).show()
        }
        return isStarted
    }

    private fun stopNodeB() {
        Log.d("MainActivity", "Остановка Узла B")
        nodeB.stopNativeProcessor()
        Toast.makeText(this, "Узел B остановлен", Toast.LENGTH_SHORT).show()
    }

    override fun onDestroy() {
        super.onDestroy()
        nodeA.stopNativeClient()
        nodeB.stopNativeProcessor()
        Log.d("MainActivity", "Приложение остановлено")
    }
}