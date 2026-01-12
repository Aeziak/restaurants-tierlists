# Dokploy Quick Start Guide

## Step 1: Prepare Dokploy Server

1. Install Dokploy on your server (follow official docs)
2. Access Dokploy UI at `https://your-dokploy-server.com`
3. Create admin account

## Step 2: Create Project

1. Click "New Project"
2. Name: `restaurant-tier-list`
3. Select "Docker Compose" type
4. Click "Create"

## Step 3: Connect Git Repository

1. Go to project settings
2. Click "Connect Repository"
3. Enter your Git repository URL
4. Add deploy key (if private repo)
5. Set branch: `main`
6. Enable "Auto deploy on push"

## Step 4: Configure Environment Variables

In Dokploy project settings, add:

```
DATABASE_NAME=restaurant_tierlist
DATABASE_USER=tierlist_user
DATABASE_PASSWORD=<generate_secure_password>
DOMAIN=your-domain.com
```

## Step 5: Configure Domain & SSL

1. Point your domain DNS A record to Dokploy server IP
2. In Dokploy project settings:
   - Domain: `your-domain.com`
   - Enable SSL
   - Select "Let's Encrypt"
   - Enable "Auto renew"

## Step 6: Initial Deployment

1. Click "Deploy" button in Dokploy UI
2. Dokploy will:
   - Clone repository
   - Build Docker images
   - Start containers
   - Configure Traefik routing
   - Request SSL certificate

## Step 7: Verify Deployment

1. Visit `https://your-domain.com`
2. Check health: `https://your-domain.com/api/v1/health`
3. View logs in Dokploy UI if issues occur

## Step 8: Setup Automatic Deployments

1. Configure webhook in your Git provider:
   - URL: `https://your-dokploy-server.com/webhook/<project-id>`
   - Secret: (from Dokploy project settings)
   - Events: Push to main branch

2. Test by pushing to main:
   ```bash
   git push origin main
   ```

3. Watch deployment in Dokploy UI

## Rollback if Needed

1. Go to project → Deployments tab
2. Find previous successful deployment
3. Click "Rollback"

## Monitoring

- **Logs**: Project → Logs tab
- **Metrics**: Project → Metrics tab
- **Health**: Check `/api/v1/health` endpoint

## Troubleshooting

### Build fails
- Check Dockerfile syntax
- Verify all files committed to Git
- Check build logs in Dokploy UI

### Container won't start
- Check environment variables
- Review container logs
- Verify database connection

### SSL certificate issues
- Ensure domain DNS is correctly configured
- Wait 5-10 minutes for propagation
- Check Let's Encrypt rate limits

### Health check fails
- Verify PostgreSQL container is running
- Check database credentials
- Review backend logs

## Production Checklist

- [ ] Domain configured with SSL
- [ ] Environment variables set
- [ ] Automatic deployment enabled
- [ ] Backup strategy configured
- [ ] Monitoring alerts set up
- [ ] Rate limiting verified
- [ ] Health checks passing
