# Dokploy Deployment Configuration

This document describes the Dokploy deployment setup for Restaurant Tier List.

## Prerequisites

- Dokploy server instance configured
- Git repository connected to Dokploy
- Domain name configured with DNS pointing to Dokploy server
- Let's Encrypt SSL certificates

## Deployment Architecture

### Services

1. **Frontend** (Next.js)
   - Port: 3000 (internal)
   - Public URL: https://yourdomain.com
   - Traefik routes HTTPS → frontend:3000

2. **Backend** (Drogon C++)
   - Port: 8080 (internal)
   - Public URL: https://yourdomain.com/api
   - Traefik routes /api/* → backend:8080

3. **PostgreSQL** (Database)
   - Port: 5432 (internal only)
   - Persistent volume for data

4. **Redis** (Cache)
   - Port: 6379 (internal only)

5. **Export Service** (Node.js)
   - Port: 3001 (internal only)

### Network Configuration

- All services on `tierlist-network` bridge
- Only frontend exposed to internet via Traefik
- Backend API accessible via `/api` reverse proxy path

## Dokploy Configuration

### Project Setup

1. Create new project in Dokploy UI: "restaurant-tier-list"
2. Connect Git repository
3. Set branch: `main`
4. Enable automatic deployments on push

### Environment Variables

Configure in Dokploy UI:

```env
# Database
DATABASE_HOST=postgres
DATABASE_PORT=5432
DATABASE_NAME=restaurant_tierlist
DATABASE_USER=tierlist_user
DATABASE_PASSWORD=<secure_password>

# Redis
REDIS_HOST=redis
REDIS_PORT=6379

# Backend
BACKEND_URL=http://backend:8080

# Frontend
NEXT_PUBLIC_BACKEND_URL=/api
NODE_ENV=production

# Domain
DOMAIN=yourdomain.com
```

### Traefik Labels

Configure in `docker-compose.prod.yml`:

```yaml
services:
  frontend:
    labels:
      - "traefik.enable=true"
      - "traefik.http.routers.frontend.rule=Host(`yourdomain.com`)"
      - "traefik.http.routers.frontend.entrypoints=websecure"
      - "traefik.http.routers.frontend.tls.certresolver=letsencrypt"
      - "traefik.http.services.frontend.loadbalancer.server.port=3000"
      
  backend:
    labels:
      - "traefik.enable=true"
      - "traefik.http.routers.backend.rule=Host(`yourdomain.com`) && PathPrefix(`/api`)"
      - "traefik.http.routers.backend.entrypoints=websecure"
      - "traefik.http.routers.backend.tls.certresolver=letsencrypt"
      - "traefik.http.services.backend.loadbalancer.server.port=8080"
```

## CI/CD Pipeline

### Automatic Deployment Flow

1. Developer pushes to `main` branch
2. Dokploy detects git webhook
3. Dokploy pulls latest code
4. Dokploy builds Docker images:
   - Frontend: `Dockerfile.frontend`
   - Backend: `backend/Dockerfile`
   - Export Service: `export-service/Dockerfile`
5. Dokploy stops old containers (graceful shutdown)
6. Dokploy starts new containers
7. Traefik automatically routes to new containers
8. Health check verifies deployment

### Rollback Procedure

If deployment fails:
1. Go to Dokploy UI → Projects → restaurant-tier-list
2. Click "Deployments" tab
3. Select previous successful deployment
4. Click "Rollback"
5. Dokploy restores previous container versions

## Security

### SSL/TLS Configuration

- Let's Encrypt automatic certificate renewal
- TLS 1.3+ enforced
- HSTS headers enabled
- Secure ciphers only

### Rate Limiting

Configured in Traefik middleware:

```yaml
http:
  middlewares:
    rate-limit:
      rateLimit:
        average: 100
        period: 1m
        burst: 50
```

### Firewall Rules

- Port 80: Redirect to 443
- Port 443: HTTPS traffic (Traefik)
- Port 5432, 6379, 8080, 3001: Internal only
- SSH: Restricted to admin IPs

## Monitoring

### Health Checks

- Frontend: HTTP GET / → 200
- Backend: HTTP GET /api/v1/health → 200
- Export Service: HTTP GET /health → 200

### Logs

Access logs via Dokploy UI:
1. Projects → restaurant-tier-list
2. Select service (frontend, backend, etc.)
3. Click "Logs" tab

## Production Checklist

- [ ] Dokploy server provisioned and secured
- [ ] Domain DNS configured
- [ ] Git repository connected
- [ ] Environment variables set
- [ ] SSL certificates configured
- [ ] Health checks passing
- [ ] Rate limiting enabled
- [ ] Backup strategy configured for PostgreSQL
- [ ] Monitoring alerts configured
- [ ] Rollback tested

## Future Improvements

- Implement blue-green deployment strategy
- Add automated testing in CI/CD pipeline
- Configure database backup automation
- Set up application performance monitoring (APM)
- Implement CDN for static assets
