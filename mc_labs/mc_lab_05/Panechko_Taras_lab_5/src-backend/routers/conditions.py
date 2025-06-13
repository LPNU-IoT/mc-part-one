from datetime import datetime

from fastapi import APIRouter, Depends
from sqlmodel import Session

from src.controllers.conditions import get_conditions
from src.core.config import settings
from src.db_models.conditions import Condition


router = APIRouter(prefix="/api/v1", tags=["conditions"])


@router.get("/conditions")
async def get_condition_records(
        start_date: datetime | None = None,
        end_date: datetime | None = None,
        is_last_updated_values: bool = False,
        without_local: bool = False,
        session: Session = Depends(settings.get_sqlite_session)
) -> list[Condition]:
    return await get_conditions(
        start_date=start_date,
        end_date=end_date,
        is_last_updated_values=is_last_updated_values,
        without_local=without_local,
        session=session
    )
