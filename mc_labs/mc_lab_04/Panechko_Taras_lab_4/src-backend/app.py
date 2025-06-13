import threading
from contextlib import asynccontextmanager
from typing import AsyncIterator

from fastapi import FastAPI

import paho.mqtt.client as mqtt
from starlette.middleware.cors import CORSMiddleware

from controllers.mqtt import on_connect, on_message
from core.config import settings
from routers.conditions import router as conditions_router


def start_mqtt():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(settings.MQTT_HOST, settings.MQTT_PORT, settings.MQTT_KEEP_ALIVE)
    client.loop_forever()


@asynccontextmanager
async def lifespan(app: FastAPI) -> AsyncIterator[None]:
    settings.create_db_and_tables()
    threading.Thread(target=start_mqtt, daemon=True).start()
    yield


app = FastAPI(lifespan=lifespan)

origins = [
    "http://localhost:3000",
    "http://localhost:8000",
]

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

app.include_router(conditions_router)
#docker start mqtt-broker
