import paho.mqtt.client as mqtt
import mysql.connector

# Conexão com banco MySQL
con = mysql.connector.connect(
    host='localhost',
    database='bancoiot',
    user='root',
    password=''#senha do seu mysql
)

if con.is_connected():
    print("✅ Conectado com sucesso ao MySQL!")

    cursor = con.cursor()
    cursor.execute("SELECT DATABASE();")
    linha = cursor.fetchone()
    print("📂 Banco em uso:", linha)

    create_table = """
        CREATE TABLE IF NOT EXISTS dados_sensor (
            id INT AUTO_INCREMENT PRIMARY KEY,
            distancia_frontal VARCHAR(255),
            distancia_traseira VARCHAR(255),
            temperatura VARCHAR(255),
            luminosidade VARCHAR(255),
            timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    """
    try:
        cursor.execute(create_table)
        print("🆗 Tabela verificada/criada.")
    except Exception as e:
        print("⚠️ Erro ao criar tabela:", e)

    cursor.close()

# Leituras parciais
leituras_parciais = {
    "temperatura": None,
    "distancia_frontal": None,
    "distancia_traseira": None,
    "luminosidade": None
}

# Callback de conexão MQTT
def on_connect(client, userdata, flags, rc):
    print("📡 Conectado ao broker MQTT com código:", rc)
    client.subscribe("topico_sensor_temperatura")
    client.subscribe("topico_sensor_luminosidade")
    client.subscribe("topico_sensor_ultrassonico_frente")
    client.subscribe("topico_sensor_ultrassonico_traseiro")

# Callback de recebimento de mensagens MQTT
def on_message(client, userdata, msg):
    global leituras_parciais
    valor = msg.payload.decode().strip()
    print(f"[{msg.topic}] → {valor}")

    cursor = con.cursor()

    if msg.topic == "topico_sensor_temperatura":
        leituras_parciais["temperatura"] = valor
    elif msg.topic == "topico_sensor_ultrassonico_frente":
        leituras_parciais["distancia_frontal"] = valor
    elif msg.topic == "topico_sensor_ultrassonico_traseiro":
        leituras_parciais["distancia_traseira"] = valor
    elif msg.topic == "topico_sensor_luminosidade":
        if valor == "termina":
            print("🚪 Encerrando conexão com o banco.")
            if con.is_connected():
                con.close()
                print("✔ Conexão encerrada.")
            return
        elif valor == "delete":
            cursor.execute("TRUNCATE TABLE dados_sensor")
            con.commit()
            print("🧹 Tabela limpa.")
            cursor.close()
            return
        else:
            leituras_parciais["luminosidade"] = valor

    print("📋 Leituras atuais:", leituras_parciais)

    # Se todas as leituras foram preenchidas, insere no BD
    if all(leituras_parciais.values()):
        try:
            cursor.execute("""
                INSERT INTO dados_sensor (distancia_frontal, distancia_traseira, temperatura, luminosidade)
                VALUES (%s, %s, %s, %s)
            """, (
                leituras_parciais["distancia_frontal"],
                leituras_parciais["distancia_traseira"],
                leituras_parciais["temperatura"],
                leituras_parciais["luminosidade"]
            ))
            con.commit()
            print("✅ Dados inseridos com sucesso:", leituras_parciais)
        except Exception as e:
            print("❌ Erro ao inserir dados:", e)

        # Limpa para novo ciclo
        leituras_parciais = {
            "temperatura": None,
            "distancia_frontal": None,
            "distancia_traseira": None,
            "luminosidade": None
        }

    cursor.close()

# Execução principal
if __name__ == '__main__':
    print("🚀 Iniciando cliente MQTT...")
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect("test.mosquitto.org", 1883, 60)

    try:
        client.loop_forever()
    except KeyboardInterrupt:
        print("🛑 Encerrando programa.")
        if con.is_connected():
            con.close()
            print("✔ Conexão com banco finalizada.")
