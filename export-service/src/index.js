import express from 'express';

const app = express();
const PORT = process.env.PORT || 3001;

app.use(express.json());

// Health check
app.get('/health', (req, res) => {
  res.json({ status: 'ok', service: 'export-service' });
});

// Placeholder for image generation endpoint
app.post('/generate-image', (req, res) => {
  res.status(501).json({ 
    error: 'Not implemented yet',
    message: 'Image generation will be implemented in Story 7.2'
  });
});

app.listen(PORT, () => {
  console.log(`Export service running on port ${PORT}`);
});
