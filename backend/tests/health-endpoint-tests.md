# Health Check Endpoint Tests

## Manual Testing

### Test 1: Health Check - All Services Up

```bash
curl http://localhost:8080/api/v1/health
```

**Expected Response (HTTP 200):**
```json
{
  "status": "healthy",
  "timestamp": "2026-01-12T10:30:00Z",
  "services": {
    "postgres": "up",
    "redis": "up"
  }
}
```

### Test 2: Health Check - PostgreSQL Down

Stop postgres container:
```bash
docker-compose stop postgres
```

Test endpoint:
```bash
curl http://localhost:8080/api/v1/health
```

**Expected Response (HTTP 503):**
```json
{
  "status": "degraded",
  "timestamp": "2026-01-12T10:31:00Z",
  "services": {
    "postgres": "down",
    "redis": "up"
  }
}
```

Restart postgres:
```bash
docker-compose start postgres
```

### Test 3: Response Time Check

```bash
time curl http://localhost:8080/api/v1/health
```

**Expected:** < 100ms when all services are up

## Docker Testing

With docker-compose running:
```bash
docker-compose up -d
curl http://localhost:8080/api/v1/health
```

## Notes

- Redis check is placeholder (returns "up" by default)
- Full Redis integration will be added when redis-plus-plus is properly configured
- PostgreSQL check uses simple `SELECT 1` query
