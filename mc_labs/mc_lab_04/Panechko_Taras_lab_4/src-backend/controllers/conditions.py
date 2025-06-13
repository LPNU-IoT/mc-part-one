from datetime import datetime, timezone

import openmeteo_requests
from sqlmodel import Session, select, func

from core.config import settings
from db_models.conditions import Condition


openmeteo = openmeteo_requests.Client(session=settings.get_retry_cache_session())


async def get_prometeo_conditions() -> Condition:
    params = {**settings.OPENMETEO_WHETHER_CONDITIONS, **settings.LVIV}
    responses = openmeteo.weather_api(settings.OPENMETEO_URL, params=params)
    current = responses[0].Current()
    temperature = current.Variables(0).Value()
    humidity = current.Variables(1).Value()

    return Condition(
        device_name="prometeo",
        location="Lviv",
        temperature=temperature,
        humidity=humidity,
        created_at=datetime.now(timezone.utc)
    )


async def get_conditions(
    without_local: bool,
    start_date: datetime | None,
    end_date: datetime | None,
    is_last_updated_values: bool,
    session: Session
) -> list[Condition]:
    external_condition = await get_prometeo_conditions()
    if without_local:
        return [external_condition]

    query = select(Condition).where(
        *((Condition.created_at >= start_date,) if start_date else ()) +
         ((Condition.created_at <= end_date,) if end_date else ()) +
         ((Condition.id.in_(
             select(func.max(Condition.id)).group_by(Condition.device_name)
         ),) if is_last_updated_values else ())
    )
    local_conditions = list(session.exec(query).all())
    local_conditions.append(external_condition)
    return local_conditions
