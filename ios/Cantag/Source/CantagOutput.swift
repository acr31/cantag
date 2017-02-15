import AVFoundation

public protocol CantagDelegate: class {
    func cantagDidOutputMetadata() // todo
    func cantagDidProcessedFrame()
    func cantagDidOutputDebugBuffer(data: UnsafeMutableRawPointer, width: Int, height: Int)
}

extension CantagDelegate {
    func cantagDidProcessedFrame() {}
    func cantagDidOutputDebugBuffer(data: UnsafeMutableRawPointer, width: Int, height: Int) {}
}

public class CantagOutput: AVCaptureVideoDataOutput, AVCaptureVideoDataOutputSampleBufferDelegate {
    
    public var debug: Bool = false;
    public var delegate: CantagDelegate?
    fileprivate var debugBuffer: UnsafeMutableRawPointer?
    
    private var debugBufferSize: Int = 0 {
        didSet {
            if debugBufferSize > 0 {
                if debugBuffer != nil {
                    debugBuffer!.deallocate(bytes: debugBufferSize, alignedTo: 1)
                }
                
                debugBuffer = UnsafeMutableRawPointer.allocate(bytes: debugBufferSize, alignedTo: 1)
            } else {
                debugBuffer?.deallocate(bytes: debugBufferSize, alignedTo: 1)
            }
        }
    }
    
    override init() {
        super.init()
        
        videoSettings = [ kCVPixelBufferPixelFormatTypeKey as AnyHashable : kCVPixelFormatType_32BGRA ]
        alwaysDiscardsLateVideoFrames = true
        
        let queue = DispatchQueue(label: "com.github.acr31.Cantag", attributes: []);
        setSampleBufferDelegate(self, queue: queue)
    }
    
    public func captureOutput(_ captureOutput: AVCaptureOutput!, didOutputSampleBuffer sampleBuffer: CMSampleBuffer!, from connection: AVCaptureConnection!) {
        
        let imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer)
        let dataSize = CVPixelBufferGetDataSize(imageBuffer!)
        let width = CVPixelBufferGetWidth(imageBuffer!)
        let height = CVPixelBufferGetHeight(imageBuffer!)
        let bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer!)
        
        guard imageBuffer != nil else {
            return;
        }
        
        if debug && debugBufferSize < dataSize {
            debugBufferSize = dataSize;
        }
        
        CVPixelBufferLockBaseAddress(imageBuffer!, CVPixelBufferLockFlags.readOnly)
        
        let baseAddress = CVPixelBufferGetBaseAddress(imageBuffer!)
        let data = baseAddress!.assumingMemoryBound(to: UInt8.self)
        let debugData = debugBuffer?.assumingMemoryBound(to: UInt8.self)
        
        CantagWrapper.process(Int32(width), Int32(height), Int32(bytesPerRow), data, debugData)
        
        self.delegate?.cantagDidProcessedFrame()
        
        if debug {
            DispatchQueue.global(qos: .background).async {
                self.delegate?.cantagDidOutputDebugBuffer(data: self.debugBuffer!, width: width, height: height)
            }
        }
        
        CVPixelBufferUnlockBaseAddress(imageBuffer!, CVPixelBufferLockFlags.readOnly)
    }
}
