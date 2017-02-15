import AVFoundation
import UIKit

class ViewController: UIViewController {
    
    fileprivate var captureSession = AVCaptureSession()
    fileprivate var cantagOutput = CantagOutput()
    fileprivate var previewLayer = AVCaptureVideoPreviewLayer()
    fileprivate var cameraView = UIView()
    fileprivate var lastUpdateTime: TimeInterval = 0
    fileprivate var debugImageView = UIImageView()
    fileprivate var fpsLabel = UILabel()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        cameraView.frame = view.frame
        debugImageView.frame = view.frame
        fpsLabel.frame = CGRect(x: 5, y: 20, width: view.frame.width, height: 20)
        fpsLabel.textColor = UIColor.black
        fpsLabel.font = UIFont.boldSystemFont(ofSize: 10)
        
        view.addSubview(cameraView)
        view.addSubview(debugImageView)
        view.addSubview(fpsLabel)
        
        debugImageView.contentMode = .scaleAspectFit
        //debugImageView.alpha = 0.5
        
        configureCamera()
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        captureSession.startRunning()
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        captureSession.stopRunning()
    }
    
    private func configureCamera() {
        cantagOutput.delegate = self
        cantagOutput.debug = true
        
        captureSession.sessionPreset = AVCaptureSessionPreset640x480
        
        let deviceSession = AVCaptureDeviceDiscoverySession(deviceTypes: [.builtInDualCamera, .builtInTelephotoCamera, .builtInWideAngleCamera], mediaType: AVMediaTypeVideo, position: .back)
        
        if let device = deviceSession?.devices.first {
            do {
                let input = try AVCaptureDeviceInput(device: device)
                
                if captureSession.canAddInput(input) {
                    captureSession.addInput(input)
                    
                    if captureSession.canAddOutput(cantagOutput) {
                        captureSession.addOutput(cantagOutput)
                        
                        previewLayer = AVCaptureVideoPreviewLayer(session: captureSession)
                        previewLayer.videoGravity = AVLayerVideoGravityResizeAspect
                        previewLayer.connection.videoOrientation = .portrait
                        
                        previewLayer.frame = cameraView.layer.bounds
                        cameraView.layer.addSublayer(previewLayer)
                        
                        cantagOutput.connection(withMediaType: AVMediaTypeVideo)?.videoOrientation = .portrait
                    }
                }
            } catch let error {
                print(error)
            }
        }
    }
}

extension ViewController: CantagDelegate {
    
    func cantagDidOutputMetadata() {
        // todo
    }
    
    func cantagDidProcessedFrame() {
        let currentTime = Date().timeIntervalSince1970
        let dt = currentTime - lastUpdateTime
        let currentFPS = Int(1 / dt)
        lastUpdateTime = currentTime
             
        DispatchQueue.main.async {
            self.fpsLabel.text = "fps: \(currentFPS)"
        }
    }
    
    func cantagDidOutputDebugBuffer(data: UnsafeMutableRawPointer, width: Int, height: Int) {
        let context = CGContext(data: data, width: width, height: height, bitsPerComponent: 8, bytesPerRow: width, space: CGColorSpaceCreateDeviceGray(), bitmapInfo: CGImageAlphaInfo.none.rawValue)
        
        let cgImage = context!.makeImage()
        
        DispatchQueue.main.async {
            self.debugImageView.image = UIImage(cgImage: cgImage!)
        }
    }
}
