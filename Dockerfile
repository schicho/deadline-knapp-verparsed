FROM python:3.12-slim

WORKDIR /app

ENV VIRTUAL_ENV=/opt/venv
ENV PATH="$VIRTUAL_ENV/bin:$PATH"
ENV PYTHONDONTWRITEBYTECODE=1
ENV PYTHONUNBUFFERED=1
ENV FLASK_DEBUG=0

RUN python -m venv "$VIRTUAL_ENV"

COPY webservice/requirements.txt /tmp/requirements.txt
RUN pip install --upgrade pip \
	&& pip install --no-cache-dir -r /tmp/requirements.txt

COPY . /app

EXPOSE 5000

CMD ["python", "webservice/app.py"]
