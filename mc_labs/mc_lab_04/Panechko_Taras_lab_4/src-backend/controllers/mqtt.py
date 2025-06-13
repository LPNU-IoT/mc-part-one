import json
import logging

from sqlmodel import Session

from core.config import settings
from db_models.conditions import Condition
from pydantic import ValidationError
from json.decoder import JSONDecodeError

logger = logging.getLogger(__name__)


def on_connect(client, userdata, flags, rc):
    logger.info(f"Connected to MQTT broker with result code {rc}")
    client.subscribe(settings.MQTT_TOPIC)
    logger.info(f"Subscribed to topic: {settings.MQTT_TOPIC}")


def on_message(client, userdata, msg):
    try:
        raw_payload = msg.payload.decode()
        data = json.loads(raw_payload)
        payload = Condition.model_validate(data)
        with Session(settings.get_engine()) as session:
            session.add(payload)
            session.commit()
        logger.info(f"Saved payload from topic '{msg.topic}': {data}")
    except JSONDecodeError as e:
        logger.error(f"JSON decode error: {e}. Raw message: {msg.payload}")
    except ValidationError as e:
        logger.error(f"Validation error: {e.errors()}. Payload: {msg.payload}")
    except Exception as e:
        logger.exception(f"Unexpected error while processing MQTT message: {e}")